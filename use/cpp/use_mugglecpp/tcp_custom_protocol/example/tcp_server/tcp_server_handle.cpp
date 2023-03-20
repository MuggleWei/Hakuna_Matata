#include "tcp_server_handle.h"
#include "muggle/cpp/muggle_cpp.h"
#include "server_peer.h"

TcpServerHandle::TcpServerHandle()
	: dispatcher_(nullptr)
	, last_timer_trigger_(0)
{}
TcpServerHandle::~TcpServerHandle()
{}

void TcpServerHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);

	LOG_INFO("TCP Listen handle add context");
}
void TcpServerHandle::OnConnect(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("connection: addr=%s", addr);

	ServerPeer *peer = new ServerPeer();
	Session *session = new Session;

	ctx->SetUserData(session);

	session->setUserData(peer);
	session->initBytesBuffer(4 * 1024 * 1024);
	session->setDispatcher(dispatcher_);
	session->setSocketContext(ctx);
	session->updateActiveTime(time(NULL));
	session->setAddr(addr);

	peer->setSession(session);

	ctx_set_.insert(ctx);
}
void TcpServerHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
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
void TcpServerHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	LOG_INFO("disconnection: addr=%s", session->getAddr());
}
void TcpServerHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	ServerPeer *peer = (ServerPeer*)session->getUserData();

	LOG_INFO("release, addr=%s", session->getAddr());

	delete peer;
	delete session;
	ctx->Close();
	ctx_set_.erase(ctx);
}
void TcpServerHandle::OnTimer(NetEventLoop *evloop)
{
	MUGGLE_UNUSED(evloop);

	time_t curr_ts = time(NULL);
	time_t elapsed = curr_ts - last_timer_trigger_;
	if (elapsed < 5)
	{
		return;
	}
	last_timer_trigger_ = curr_ts;

	for (SocketContext *ctx : ctx_set_)
	{
		Session *session = (Session*)ctx->GetUserData();
		time_t elapsed = curr_ts - session->lastActiveTime();
		if (elapsed > 15)
		{
			ctx->Shutdown();
		}
	}
}

void TcpServerHandle::setDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
