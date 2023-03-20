#include "client_peer.h"
#include "muggle/cpp/muggle_cpp.h"
#include "demo_msg.h"

#define CALLBACK_IMPL(name, msg_type) \
void ClientPeer::s_##name(Session *session, void *msg) \
{ \
	ClientPeer *peer = (ClientPeer*)session->getUserData(); \
	if (peer == nullptr) \
	{ \
		LOG_ERROR("failed get peer"); \
		return; \
	} \
	peer->name((msg_type*)msg); \
} \
\
void ClientPeer::name(msg_type *msg)

//////////////// constructor & destructor ////////////////
ClientPeer::ClientPeer()
	: session_(nullptr)
	, req_id_(0)
	, is_logined_(false)
{}
ClientPeer::~ClientPeer()
{}

//////////////// methods ////////////////

void ClientPeer::setSession(Session *session)
{
	session_ = session;
}

void ClientPeer::onConnection()
{
	demo_msg_req_login_t req;
	memset(&req, 0, sizeof(req));
	req.hdr.msg_id = DEMO_MSG_ID_REQ_LOGIN;
	req.req_id = req_id_++;
	strncpy(req.user_id, "foo", sizeof(req.user_id) - 1);
	strncpy(req.password, "123456", sizeof(req.password) - 1);

	LOG_INFO("req login: addr=%s, user_id=%s",
		session_->getAddr(), req.user_id);

	session_->sendMessage(&req, sizeof(req));
}
void ClientPeer::onTimer()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	demo_msg_ping_t req;
	memset(&req, 0, sizeof(req));
	req.hdr.msg_id = DEMO_MSG_ID_PING;
	req.sec = (uint64_t)ts.tv_sec;
	req.nsec = (uint32_t)ts.tv_nsec;

	LOG_INFO("send req ping: addr=%s, sec=%llu, nsec=%09lu",
		session_->getAddr(),
		(unsigned long long)req.sec, (unsigned long)req.nsec);

	session_->sendMessage(&req, sizeof(req));

	if (is_logined_)
	{
		const static uint32_t max_sum_len = 10;
		uint32_t cnt = ((uint32_t)rand()) % max_sum_len + 1;
		uint32_t datalen = sizeof(demo_msg_req_sum_t) + sizeof(uint32_t) * cnt;

		char str[max_sum_len * 5 + 1];
		memset(str, 0, sizeof(str));
		char *p = str;

		demo_msg_req_sum_t *req = (demo_msg_req_sum_t*)malloc(datalen);
		memset(req, 0, sizeof(*req));

		uint32_t *arr = (uint32_t*)(req + 1);
		for (uint32_t i = 0; i < cnt; i++)
		{
			arr[i] = (int32_t)((uint32_t)rand() % 201) - 100;
			int offset = snprintf(p, 5, "%d ", arr[i]);
			p += offset;
		}

		req->hdr.msg_id = DEMO_MSG_ID_REQ_SUM;
		req->req_id = req_id_++;
		req->arr_len = cnt;

		LOG_INFO(
			"send request sum message: "
			"req_id=%lu, req_array=[%s]",
			(unsigned long)req->req_id, str);

		session_->sendMessage(req, datalen);

		free(req);
	}
}

//////////////// callbacks ////////////////
CALLBACK_IMPL(onPong, demo_msg_pong_t)
{
	LOG_INFO("recv rsp pong: addr=%s, sec=%llu, nsec=%09lu",
		session_->getAddr(),
		(unsigned long long)msg->sec, (unsigned long)msg->nsec);

	session_->updateActiveTime(time(NULL));
}
CALLBACK_IMPL(onRspLogin, demo_msg_rsp_login_t)
{
	if (!msg->login_result)
	{
		LOG_ERROR("login failed");
		session_->Close();
		return;
	}
	is_logined_ = true;

	LOG_INFO("recv rsp login success");
}
CALLBACK_IMPL(onRspSum, demo_msg_rsp_sum_t)
{
	LOG_INFO(
		"recv response sum message: "
		"req_id=%lu, sum=%ld",
		(unsigned long)msg->req_id, (long)msg->sum);
}
