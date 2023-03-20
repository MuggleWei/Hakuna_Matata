#ifndef TCP_LISTEN_HANDLE_H_
#define TCP_LISTEN_HANDLE_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include "muggle/cpp/net/net_event_handle.h"
#include "muggle/cpp/net/net_event_loop.h"
#include "demo/dispatcher.h"

USING_NS_MUGGLE;
USING_NS_MUGGLE_DEMO;

class TcpListenHandle : public NetEventHandle
{
public:
	TcpListenHandle();
	virtual ~TcpListenHandle();

	void SetWorkers(std::vector<NetEventLoop*> *workers);

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnConnect(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void setDispatcher(Dispatcher *dispatcher);

private:
	int worker_idx_;
	Dispatcher *dispatcher_;
	std::vector<NetEventLoop*> *worker_evloops_;
	std::map<std::string, int> user_worker_dict_;
	std::set<SocketContext*> ctx_set_;
};

#endif // !MUGGLE_DEMO_LISTEN_HANDLE_H_
