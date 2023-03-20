#ifndef TCP_CLIENT_HANDLE_H_
#define TCP_CLIENT_HANDLE_H_

#include "demo/session.h"
#include "muggle/cpp/net/net_event_handle.h"
#include "muggle/cpp/net/net_event_loop.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

class TcpClientHandle : public NetEventHandle
{
public:
	TcpClientHandle();
	virtual ~TcpClientHandle();

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void setServerAddr(const char *host, const char *port);
	void setEventLoop(NetEventLoop *evloop);
	void setDispatcher(Dispatcher *dispatcher);

	void connect();

private:
	std::string host_;
	std::string port_;

	NetEventLoop *evloop_;
	Dispatcher *dispatcher_;

	Session *session_;

	time_t last_timer_trigger_;
};

#endif // !TCP_CLIENT_HANDLE_H_
