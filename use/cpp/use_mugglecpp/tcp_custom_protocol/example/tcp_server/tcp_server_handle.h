#ifndef TCP_SERVER_HANDLE_H_
#define TCP_SERVER_HANDLE_H_

#include <set>
#include "demo/dispatcher.h"
#include "muggle/cpp/net/net_event_handle.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

class TcpServerHandle : public NetEventHandle
{
public:
	TcpServerHandle();
	virtual ~TcpServerHandle();

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnConnect(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void setDispatcher(Dispatcher *dispatcher);

private:
	Dispatcher *dispatcher_;
	std::set<SocketContext*> ctx_set_;
	time_t last_timer_trigger_;
};

#endif // !TCP_SERVER_HANDLE_H_
