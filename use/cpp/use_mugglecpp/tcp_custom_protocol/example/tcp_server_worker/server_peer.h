#ifndef TCP_SERVER_PEER_H_
#define TCP_SERVER_PEER_H_

#include "demo/session.h"
#include "demo_msg.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

class ServerPeer
{
public:
	//////////////// constructor & destructor ////////////////
	ServerPeer();
	virtual ~ServerPeer();

	//////////////// methods ////////////////
	void setSession(Session *session);

	void setUserID(const char *user_id);
	const std::string& getUserID();

	//////////////// callbacks ////////////////
	void onPing(demo_msg_ping_t *msg);
	void onLogin(demo_msg_req_login_t *msg);
	void onReqSum(demo_msg_req_sum_t *msg);

private:
	Session *session_;
	std::string user_id_;
	bool is_logined_;
};

#endif // !TCP_SERVER_PEER_H_
