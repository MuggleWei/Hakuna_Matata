#include "tcp_server_handle.h"
#include "muggle/cpp/muggle_cpp.h"
#include "demo/session.h"
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

	Session *session = (Session*)ctx->GetUserData();
	LOG_INFO("event loop add context, addr=%s", session->getAddr());

	ctx_set_.insert(ctx);
}
void TcpServerHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	if (!session->handleMessage())
	{
		LOG_WARNING("failed handle message, addr=%s", session->getAddr());
		ctx->Shutdown();
	}
}
void TcpServerHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	LOG_INFO("disconnect, addr=%s", session->getAddr());
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
