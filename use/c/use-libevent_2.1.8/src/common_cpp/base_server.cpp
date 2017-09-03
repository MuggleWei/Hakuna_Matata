#include "base_server.h"
#ifndef WIN32
#include <signal.h>
#endif
#include <string.h>
#include "common/utils.h"
#include "common/peer.h"
#include "common/object.h"
#include "common_cpp/peer_io_thread.h"

static void do_init(struct Peer *peer, struct PeerIOThread_AddSocket * task)
{
	peer->extra = task->extra;
	PeerIOThread *io_thread = (PeerIOThread*)peer->base_info.thread;
	fprintf(stdout, "#%d Thread has peer: %d<%s>\n", 
		io_thread->thread_id, (int)io_thread->peers.size(), peer->base_info.remote_addr);
}
static void do_accept(evutil_socket_t listener, short /*event*/, void *arg)
{
	struct sockaddr_storage ss;
	socklen_t slen = sizeof(ss);
	evutil_socket_t fd = accept(listener, (struct sockaddr*)&ss, &slen);
	if (fd < 0)
	{
		perror("accept");
	}
	else
	{
		FuncAcceptCb &on_accept = *(FuncAcceptCb*)arg;
		on_accept(fd, &ss, slen);
	}
}
static void do_read(struct Peer *peer, short /*events*/)
{
	BaseServer *serv = (BaseServer*)peer->extra;
	serv->onRead(peer);
}
static void do_write(struct Peer *peer, short /*events*/)
{
	BaseServer *serv = (BaseServer*)peer->extra;
	serv->onWrite(peer);
}
static void do_event(struct Peer *peer, short events)
{
	BaseServer *serv = (BaseServer*)peer->extra;
	serv->onEvent(peer, events);
}

BaseServer::BaseServer()
	: in_timeout_(0)
	, out_timeout_(0)
	, checktimout_interval_(15)
	, read_cb_(nullptr)
	, write_cb_(nullptr)
	, event_cb_(nullptr)
	, accept_cb_(std::bind(&BaseServer::onAccept, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
{}
BaseServer::~BaseServer()
{
	// TODO: 
}

void BaseServer::IOThread(int num)
{
	io_thread_num_ = num;
	io_thread_num_ = io_thread_num_ >= 1 ? io_thread_num_ : 1;
}
void BaseServer::IdleTimeout(unsigned short in_timeout, unsigned short out_timeout)
{
	in_timeout_ = in_timeout;
	out_timeout_ = out_timeout;
}
void BaseServer::RegistReadCb(std::function<void(struct Peer*)>& cb)
{
	read_cb_ = cb;
}
void BaseServer::RegistWriteCb(std::function<void(struct Peer*)>& cb)
{
	write_cb_ = cb;
}
void BaseServer::RegistEventCb(std::function<void(struct Peer*, short)>& cb)
{
	event_cb_ = cb;
}

void BaseServer::Run(int port)
{
#if WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return;
	}

	if (evthread_use_windows_threads() != 0)
	{
		exit(EXIT_FAILURE);
	}
#else
	signal(SIGPIPE, SIG_IGN);

	if (evthread_use_pthreads() != 0)
	{
		exit(EXIT_FAILURE);
}
#endif

	createIOThread();

	mainLoop(port);
}
void BaseServer::onRead(struct Peer* peer)
{
	if (read_cb_)
	{
		read_cb_(peer);
	}
}
void BaseServer::onWrite(struct Peer* peer)
{
	if (write_cb_)
	{
		write_cb_(peer);
	}
}
void BaseServer::onEvent(struct Peer* peer, short events)
{
	if (event_cb_)
	{
		event_cb_(peer, events);
	}

	if (events & BEV_EVENT_EOF)
	{
		fprintf(stdout, "[eof] %s\n", peer->base_info.remote_addr);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		fprintf(stdout, "[disconnect] %s\n", peer->base_info.remote_addr);
	}
	else if (events & BEV_EVENT_TIMEOUT)
	{
		fprintf(stdout, "[timeout] %s\n", peer->base_info.remote_addr);
	}
	peerClose(peer);
}
void BaseServer::onAccept(evutil_socket_t fd, struct sockaddr_storage *ss, socklen_t slen)
{
	static int io_thread_idx = 0;

	struct PeerIOThread_AddSocket task;
	memset(&task, 0, sizeof(task));
	task.fd = fd;
	task.remote_ss = *ss;
	task.remote_ss_len = slen;
	task.read_cb = do_read;
	task.write_cb = do_write;
	task.event_cb = do_event;
	task.init_cb = do_init;
	task.extra = (void*)this;

	int ret = peerIOThreadAddSocket(io_threads_[io_thread_idx++], &task);
	if (ret < 0)
	{
		fprintf(stderr, "failed add new socket\n");
		evutil_closesocket(fd);
	}
	if (io_thread_idx >= io_threads_.size())
	{
		io_thread_idx = 0;
	}
}

void BaseServer::createIOThread()
{
	for (int i = 0; i < io_thread_num_; ++i)
	{
		io_threads_.push_back(createPeerIOThread(i));
		if (checktimout_interval_ > 0)
		{
			peerIOThreadSetTimeout(io_threads_[i], checktimout_interval_, in_timeout_, out_timeout_);
		}
	}
}
void BaseServer::mainLoop(int port)
{
	evutil_socket_t listener;
	struct sockaddr_in sin;
	struct event_base *base = NULL;
	struct event *listener_event = NULL;

	memset(&sin, 0, sizeof(sin));

	base = event_base_new();
	if (!base)
	{
		fprintf(stderr, "Failed in event_base_new()\n");
		return;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	evutil_make_socket_nonblocking(listener);

#ifndef WIN32
	{
		int one = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
#endif

	if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		perror("bind");
		return;
	}

	if (listen(listener, 16) < 0)
	{
		perror("listen");
		return;
	}

	listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)&accept_cb_);
	event_add(listener_event, NULL);

	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
}
