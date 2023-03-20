#ifndef TCP_CLIENT_PEER_H_
#define TCP_CLIENT_PEER_H_

#include "demo/session.h"
#include "demo_msg.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

#define CALLBACK_DECLARE(name, msg_type) \
static void s_##name(Session *session, void *msg); \
void name(msg_type *msg)

class ClientPeer
{
public:
	//////////////// constructor & destructor ////////////////
	ClientPeer();
	virtual ~ClientPeer();

	//////////////// methods ////////////////
	void setSession(Session *session);

	void onConnection();
	void onTimer();

	//////////////// callbacks ////////////////
	CALLBACK_DECLARE(onPong, demo_msg_pong_t);
	CALLBACK_DECLARE(onRspLogin, demo_msg_rsp_login_t);
	CALLBACK_DECLARE(onRspSum, demo_msg_rsp_sum_t);

private:
	Session *session_;
	uint32_t req_id_;
	bool is_logined_;
};

#endif // !CLIENT_PEER_H_
