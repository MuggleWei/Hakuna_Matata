#ifndef TCP_CLIENT_PEER_H_
#define TCP_CLIENT_PEER_H_

#include "demo/session.h"
#include "demo_msg.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

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
	void onPong(demo_msg_pong_t *msg);
	void onRspLogin(demo_msg_rsp_login_t *msg);
	void onRspSum(demo_msg_rsp_sum_t *msg);

private:
	Session *session_;
	uint32_t req_id_;
	bool is_logined_;
};

#endif // !CLIENT_PEER_H_
