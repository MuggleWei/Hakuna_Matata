#ifndef MUGGLE_DEMO_SESSION_H_
#define MUGGLE_DEMO_SESSION_H_

#include <string>
#include <time.h>
#include "muggle/cpp/memory/bytes_buffer.h"
#include "muggle/cpp/net/socket_context.h"
#include "demo/macro.h"

NS_MUGGLE_DEMO_BEGIN

USING_NS_MUGGLE;

class Dispatcher;

class Session
{
public:
	MUGGLE_DEMO_EXPORT
	Session();
	MUGGLE_DEMO_EXPORT
	virtual ~Session();

	MUGGLE_DEMO_EXPORT
	void setUserData(void *user_data);
	MUGGLE_DEMO_EXPORT
	void* getUserData();

	MUGGLE_DEMO_EXPORT
	void setSocketContext(SocketContext *ctx);
	MUGGLE_DEMO_EXPORT
	SocketContext* getSocketContex();
	
	MUGGLE_DEMO_EXPORT
	bool initBytesBuffer(int cap);
	MUGGLE_DEMO_EXPORT
	muggle::BytesBuffer* getBytesBuffer();

	MUGGLE_DEMO_EXPORT
	void setDispatcher(Dispatcher *dispatcher);
	MUGGLE_DEMO_EXPORT
	Dispatcher* getDispatcher();

	MUGGLE_DEMO_EXPORT
	void setDetach(bool flag);
	MUGGLE_DEMO_EXPORT
	bool isDetached();

	MUGGLE_DEMO_EXPORT
	void updateActiveTime(time_t ts);
	MUGGLE_DEMO_EXPORT
	time_t lastActiveTime();

	MUGGLE_DEMO_EXPORT
	void setAddr(const char *addr);
	MUGGLE_DEMO_EXPORT
	const char* getAddr();

	MUGGLE_DEMO_EXPORT
	bool handleMessage();
	MUGGLE_DEMO_EXPORT
	bool sendMessage(void *data, uint32_t datalen);
	
	MUGGLE_DEMO_EXPORT
	void Close();

protected:
	void *user_data_;
	muggle::BytesBuffer *bytes_buf_;
	Dispatcher *dispatcher_;
	SocketContext *ctx_;

	bool detached_;
	time_t last_actived_ts_;
	std::string addr_;
};

NS_MUGGLE_DEMO_END

#endif // !MUGGLE_DEMO_SESSION_H_
