#include "tcp_client_handle.h"
#include "muggle/cpp/muggle_cpp.h"
#include <thread>
#include "demo/session.h"
#include "demo_msg.h"
#include "client_peer.h"

TcpClientHandle::TcpClientHandle()
	: evloop_(nullptr)
	, dispatcher_(nullptr)
	, session_(nullptr)
	, last_timer_trigger_(0)
{}
TcpClientHandle::~TcpClientHandle()
{}

void TcpClientHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("success connect to %s", addr);

	ClientPeer *peer = new ClientPeer();
	Session *session = new Session;

	session->initBytesBuffer(4 * 1024 * 1024);
	session->setDispatcher(dispatcher_);
	session->setSocketContext(ctx);
	session->updateActiveTime(time(NULL));
	session->setAddr(addr);
	session->setUserData(peer);

	peer->setSession(session);

	ctx->SetUserData(session);

	session_ = session;

	peer->onConnection();
}
void TcpClientHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	if (!session->handleMessage())
	{
		LOG_WARNING("failed dispatcher message, addr=%s", session->getAddr());
		ctx->Shutdown();
		return;
	}
}
void TcpClientHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);

	session_ = nullptr;

	connect();
}
void TcpClientHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	if (session)
	{
		LOG_INFO("release %s", session->getAddr());
		delete session;
	}
	ctx->SetUserData(nullptr);
}
void TcpClientHandle::OnTimer(NetEventLoop *evloop)
{
	MUGGLE_UNUSED(evloop);

	time_t curr_ts = time(NULL);
	time_t elapsed = curr_ts - last_timer_trigger_;
	if (elapsed < 3)
	{
		return;
	}
	last_timer_trigger_ = curr_ts;

	if (session_ == nullptr)
	{
		return;
	}

	ClientPeer *peer = (ClientPeer*)session_->getUserData();
	if (peer == nullptr)
	{
		return;
	}

	peer->onTimer();
}

void TcpClientHandle::setServerAddr(const char *host, const char *port)
{
	host_ = host;
	port_ = port;
}
void TcpClientHandle::setEventLoop(NetEventLoop *evloop)
{
	evloop_ = evloop;
}
void TcpClientHandle::setDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}

void TcpClientHandle::connect()
{
	std::thread th([&] {
		while (true)
		{
			SocketContext *ctx = SocketUtils::TCPConnect(
				host_.c_str(), port_.c_str(), 3);
			if (ctx == nullptr)
			{
				std::this_thread::sleep_for(std::chrono::seconds(3));
				continue;
			}
			evloop_->AddContext(ctx);
			break;
		}
	});
	th.detach();
}
