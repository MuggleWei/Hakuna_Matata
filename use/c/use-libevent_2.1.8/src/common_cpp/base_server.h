#ifndef BASE_SERVER_H_
#define BASE_SERVER_H_

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <vector>
#include <functional>
#include "common_cpp/common_cpp_export_macro.h"
#include "common_cpp/peer_io_thread.h"

typedef std::function<void(struct Peer*)> FuncReadCb;
typedef std::function<void(struct Peer*)> FuncWriteCb;
typedef std::function<void(struct Peer*, short)> FuncEventCb;
typedef std::function<void(evutil_socket_t, struct sockaddr_storage *, socklen_t)> FuncAcceptCb;

class BaseServer
{
public:
	common_cpp_EXPORT BaseServer();
	common_cpp_EXPORT virtual ~BaseServer();

	BaseServer(const BaseServer &rhs) = delete;
	BaseServer(const BaseServer &&rhs) = delete;
	BaseServer& operator=(const BaseServer &rhs) = delete;
	BaseServer& operator=(const BaseServer &&rhs) = delete;

	common_cpp_EXPORT void IOThread(int num);
	common_cpp_EXPORT void IdleTimeout(unsigned short timeout);
	common_cpp_EXPORT void RegistReadCb(std::function<void(struct Peer*)>& cb);
	common_cpp_EXPORT void RegistWriteCb(std::function<void(struct Peer*)>& cb);
	common_cpp_EXPORT void RegistEventCb(std::function<void(struct Peer*, short)>& cb);

	common_cpp_EXPORT void Run(int port);

	common_cpp_EXPORT void onRead(struct Peer* peer);
	common_cpp_EXPORT void onWrite(struct Peer* peer);
	common_cpp_EXPORT void onEvent(struct Peer* peer, short events);
	common_cpp_EXPORT void onAccept(evutil_socket_t fd, struct sockaddr_storage *ss, socklen_t slen);

private:
	void createIOThread();
	void mainLoop(int port);

private:
	std::vector<PeerIOThread*> io_threads_;
	unsigned short timeout_;
	int checktimout_interval_;
	FuncReadCb read_cb_;
	FuncWriteCb write_cb_;
	FuncEventCb event_cb_;
	FuncAcceptCb accept_cb_;

	int io_thread_num_;
};

#endif