#include "server_peer.h"
#include "muggle/cpp/muggle_cpp.h"

//////////////// constructor & destructor ////////////////
ServerPeer::ServerPeer()
	: session_(nullptr)
	, is_logined_(false)
{}
ServerPeer::~ServerPeer()
{}

//////////////// methods ////////////////

void ServerPeer::setSession(Session *session)
{
	session_ = session;
}

void ServerPeer::setUserID(const char *user_id)
{
	user_id_ = user_id;
}
const std::string& ServerPeer::getUserID()
{
	return user_id_;
}

//////////////// callbacks ////////////////
void ServerPeer::onPing(demo_msg_ping_t *msg)
{
	LOG_INFO("recv req ping: addr=%s, sec=%llu, nsec=%09lu",
		session_->getAddr(),
		(unsigned long long)msg->sec, (unsigned long)msg->nsec);

	session_->updateActiveTime(time(NULL));

	// rsp
	NEW_STACK_DEMO_MSG(DEMO_MSG_ID_PONG, demo_msg_pong_t, rsp);
	rsp->sec = (uint64_t)msg->sec;
	rsp->nsec = (uint32_t)msg->nsec;
	DEMO_SESSION_SEND_MSG(session_, rsp);
}
void ServerPeer::onLogin(demo_msg_req_login_t *msg)
{
	if (strlen(msg->user_id) >= sizeof(msg->user_id))
	{
		LOG_ERROR("user id length beyond limit");
		session_->Close();
	}

	if (strlen(msg->password) >= sizeof(msg->password))
	{
		LOG_ERROR("password length beyond limit");
		session_->Close();
	}

	LOG_INFO("recv req login: addr=%s, user_id=%s, req_id=%lu",
		session_->getAddr(), msg->user_id, (unsigned long)msg->req_id);

	user_id_ = msg->user_id;
	is_logined_ = true;

	// detach from listen evloop
	SocketContext *ctx = (SocketContext*)session_->getSocketContex();
	session_->setDetach(true);
	ctx->RefCntRetain();
	ctx->SetFlagClose();
	LOG_INFO("prapare detach session: addr=%s, user_id=%s",
		session_->getAddr(), user_id_.c_str());

	// rsp
	NEW_STACK_DEMO_MSG(DEMO_MSG_ID_RSP_LOGIN, demo_msg_rsp_login_t, rsp);
	rsp->req_id = msg->req_id;
	rsp->login_result = 1;
	DEMO_SESSION_SEND_MSG(session_, rsp);
}
void ServerPeer::onReqSum(demo_msg_req_sum_t *msg)
{
	if (msg->arr_len > 16)
	{
		LOG_ERROR("req sum array length beyond limit");
		session_->Close();
	}

	if (!is_logined_)
	{
		LOG_ERROR("req without login");
		session_->Close();
	}

	LOG_INFO("recv req sum: addr=%s, user_id=%s, req_id=%lu",
		session_->getAddr(), user_id_.c_str(), (unsigned long)msg->req_id);

	int32_t sum = 0;
	int32_t *arr = (int32_t*)(msg + 1);
	for (uint32_t i = 0; i < msg->arr_len; i++)
	{
		sum += arr[i];
	}

	// rsp
	NEW_STACK_DEMO_MSG(DEMO_MSG_ID_RSP_SUM, demo_msg_rsp_sum_t, rsp);
	rsp->req_id = msg->req_id;
	rsp->sum = sum;
	DEMO_SESSION_SEND_MSG(session_, rsp);
}
