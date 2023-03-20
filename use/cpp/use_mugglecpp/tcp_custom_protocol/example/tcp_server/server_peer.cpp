#include "server_peer.h"
#include "muggle/cpp/muggle_cpp.h"

#define CALLBACK_IMPL(name, msg_type) \
void ServerPeer::s_##name(Session *session, void *msg) \
{ \
	ServerPeer *peer = (ServerPeer*)session->getUserData(); \
	if (peer == nullptr) \
	{ \
		LOG_ERROR("failed get peer"); \
		return; \
	} \
	peer->name((msg_type*)msg); \
} \
\
void ServerPeer::name(msg_type *msg)

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
CALLBACK_IMPL(onPing, demo_msg_ping_t)
{
	LOG_INFO("recv req ping: addr=%s, sec=%llu, nsec=%09lu",
		session_->getAddr(),
		(unsigned long long)msg->sec, (unsigned long)msg->nsec);

	session_->updateActiveTime(time(NULL));

	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	// rsp
	demo_msg_pong_t rsp;
	memset(&rsp, 0, sizeof(rsp));
	rsp.hdr.msg_id = DEMO_MSG_ID_PONG;
	rsp.sec = (uint64_t)ts.tv_sec;
	rsp.nsec = (uint32_t)ts.tv_nsec;
	session_->sendMessage(&rsp, sizeof(rsp));
}

CALLBACK_IMPL(onLogin, demo_msg_req_login_t)
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

	// rsp
	demo_msg_rsp_login_t rsp;
	memset(&rsp, 0, sizeof(rsp));
	rsp.hdr.msg_id = DEMO_MSG_ID_RSP_LOGIN;
	rsp.req_id = msg->req_id;
	rsp.login_result = 1;
	session_->sendMessage(&rsp, sizeof(rsp));
}

CALLBACK_IMPL(onReqSum, demo_msg_req_sum_t)
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
	demo_msg_rsp_sum_t rsp;
	memset(&rsp, 0, sizeof(rsp));
	rsp.hdr.msg_id = DEMO_MSG_ID_RSP_SUM;
	rsp.req_id = msg->req_id;
	rsp.sum = sum;
	session_->sendMessage(&rsp, sizeof(rsp));
}
