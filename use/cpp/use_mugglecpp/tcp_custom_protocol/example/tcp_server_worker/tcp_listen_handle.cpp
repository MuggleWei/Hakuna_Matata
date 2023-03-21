#include "tcp_listen_handle.h"
#include "muggle/cpp/muggle_cpp.h"
#include "demo/session.h"
#include "server_peer.h"

USING_NS_MUGGLE_DEMO;

TcpListenHandle::TcpListenHandle()
	: worker_idx_(0)
	, dispatcher_(nullptr)
	, worker_evloops_(nullptr)
{}
TcpListenHandle::~TcpListenHandle()
{}

void TcpListenHandle::SetWorkers(std::vector<NetEventLoop*> *workers)
{
	worker_evloops_ = workers;
}

void TcpListenHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);

	LOG_INFO("TCP Listen handle add context");
}
void TcpListenHandle::OnConnect(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("New connection: addr=%s", addr);

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
void TcpListenHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
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
void TcpListenHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	if (session->isDetached())
	{
		ServerPeer *peer = (ServerPeer*)session->getUserData();
		const std::string &user_id = peer->getUserID();
		int idx = 0;
		auto it = user_worker_dict_.find(user_id);
		if (it == user_worker_dict_.end())
		{
			idx = worker_idx_++;
			idx %= worker_evloops_->size();

			user_worker_dict_[user_id] = idx;

		}
		else
		{
			idx = it->second;
		}

		LOG_INFO("move socket context to worker event loop: "
			"addr=%s, user_id=%s, worker_idx=%d",
			session->getAddr(), user_id.c_str(), idx);

		session->setDetach(false);
		ctx->ClearFlags();

		NetEventLoop *worker = (*worker_evloops_)[idx];
		worker->AddContext(ctx);

		ctx_set_.erase(ctx);
	}
}
void TcpListenHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session*)ctx->GetUserData();
	ServerPeer *peer = (ServerPeer*)session->getUserData();

	LOG_INFO("release, addr=%s", session->getAddr());

	delete peer;
	delete session;

	ctx_set_.erase(ctx);
}
void TcpListenHandle::OnTimer(NetEventLoop *evloop)
{
	MUGGLE_UNUSED(evloop);

	time_t curr_ts = time(NULL);
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

void TcpListenHandle::setDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
