#include "muggle/cpp/muggle_cpp.h"
#include <vector>
#include <thread>
#include "demo/codec_bytes.h"
#include "demo/dispatcher.h"
#include "demo_msg.h"
#include "server_peer.h"
#include "tcp_listen_handle.h"
#include "tcp_server_handle.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

#define CALLBACK_IMPL(func, msg_type) \
	static void s_##func(Session *session, void *msg) \
	{ \
		ServerPeer *peer = (ServerPeer*)session->getUserData(); \
		if (peer == nullptr) \
		{ \
			LOG_ERROR("failed get peer"); \
			return; \
		} \
		peer->func((msg_type*)((msg_hdr_t*)msg + 1)); \
	}

#define DISPATCHER_REGISTER(msg_id, func) \
	dispatcher.registerCallback(msg_id, s_##func);

typedef struct sys_args
{
	char host[64];
	char port[16];
} sys_args_t;

void parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	char str_usage[1024];
	snprintf(str_usage, sizeof(str_usage),
		"Usage: %s -H <host> -P <port>\n"
		"\t-h print help information\n"
		"\t-H listen/connect host\n"
		"\t-P listen/connect port",
		argv[0]);

	memset(args, 0, sizeof(*args));

	int opt;
	while ((opt = getopt(argc, argv, "hvt:H:P:")) != -1)
	{
		switch (opt)
		{
		case 'h':
		{
			LOG_INFO("\n%s", str_usage);
			exit(EXIT_SUCCESS);
		}break;
		case 'H':
		{
			strncpy(args->host, optarg, sizeof(args->host)-1);
		}break;
		case 'P':
		{
			strncpy(args->port, optarg, sizeof(args->port)-1);
		}break;
		}
	}

	if (args->host[0] == '\0' ||
		args->port[0] == '\0')
	{
		LOG_ERROR("\n%s", str_usage);
		exit(EXIT_FAILURE);
	}

	LOG_INFO(
		"\n"
		"----- input args -----\n"
		"host=%s\n"
		"port=%s\n"
		"----------------------",
		args->host, args->port);
}

CALLBACK_IMPL(onPing, demo_msg_ping_t);
CALLBACK_IMPL(onLogin, demo_msg_req_login_t);
CALLBACK_IMPL(onReqSum, demo_msg_req_sum_t);

void initDispatcher(Dispatcher &dispatcher)
{
	CodecBytes *codec = new CodecBytes();
	codec->setRecvUnitSize(4 * 1024);
	codec->setMsgLenLimit(32 * 1024);

	dispatcher.setTotalMessage(MAX_DEMO_MSG_ID);
	dispatcher.appendCodec(codec);

	DISPATCHER_REGISTER(DEMO_MSG_ID_PING, onPing);
	DISPATCHER_REGISTER(DEMO_MSG_ID_REQ_LOGIN, onLogin);
	DISPATCHER_REGISTER(DEMO_MSG_ID_REQ_SUM, onReqSum);
}

void run_tcp_server(const char *host, const char *port)
{
	// init dispatcher
	Dispatcher dispatcher;
	initDispatcher(dispatcher);

	// run workers
	std::vector<NetEventLoop*> worker_evloops;
	for (int i = 0; i < 1; i++)
	{
		TcpServerHandle *handle = new TcpServerHandle();
		handle->setDispatcher(&dispatcher);

		NetEventLoop *evloop = new NetEventLoop(128, 0);
		evloop->SetHandle(handle);
		evloop->SetTimerInterval(3000);
		worker_evloops.push_back(evloop);
		std::thread th([evloop] {
			evloop->Run();
		});
		th.detach();
	}

	// run listen
	TcpListenHandle handle;
	handle.SetWorkers(&worker_evloops);
	handle.setDispatcher(&dispatcher);

	NetEventLoop evloop(128, 0);
	evloop.SetHandle(&handle);
	evloop.SetTimerInterval(3000);

	SocketContext *listen_ctx = SocketUtils::TCPListen(host, port, 512);
	if (listen_ctx == nullptr)
	{
		LOG_ERROR("failed listen %s:%s", host, port);
		return;
	}

	evloop.AddContext(listen_ctx);
	evloop.Run();
}

int main(int argc, char *argv[])
{
	// init log
	if (!muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL))
	{
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	// init socket
	if (muggle_socket_lib_init() != 0)
	{
		LOG_ERROR("failed initalize socket library");
		exit(EXIT_FAILURE);
	}

	// parse system arguments
	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	// run tcp server
	run_tcp_server(args.host, args.port);
	
	return 0;
}
