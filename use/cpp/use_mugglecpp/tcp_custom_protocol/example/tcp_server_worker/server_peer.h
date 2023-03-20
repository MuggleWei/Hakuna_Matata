#ifndef TCP_SERVER_PEER_H_
#define TCP_SERVER_PEER_H_

#include "demo/session.h"
#include "demo_msg.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

#define CALLBACK_DECLARE(name, msg_type) \
static void s_##name(Session *session, void *msg); \
void name(msg_type *msg)


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
	CALLBACK_DECLARE(onPing, demo_msg_ping_t);
	CALLBACK_DECLARE(onLogin, demo_msg_req_login_t);
	CALLBACK_DECLARE(onReqSum, demo_msg_req_sum_t);

private:
	Session *session_;
	std::string user_id_;
	bool is_logined_;
};

#endif // !TCP_SERVER_PEER_H_
