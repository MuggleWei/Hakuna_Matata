#include "session.h"
#include "muggle/cpp/muggle_cpp.h"
#include "dispatcher.h"

NS_MUGGLE_DEMO_BEGIN

Session::Session()
	: user_data_(nullptr)
	, bytes_buf_(nullptr)
	, dispatcher_(nullptr)
	, ctx_(nullptr)
	, detached_(false)
	, last_actived_ts_(0)
{}
Session::~Session()
{
	if (bytes_buf_)
	{
		delete bytes_buf_;
		bytes_buf_ = nullptr;
	}
}

void Session::setUserData(void *user_data)
{
	user_data_ = user_data;
}
void* Session::getUserData()
{
	return user_data_;
}

void Session::setSocketContext(SocketContext *ctx)
{
	ctx_ = ctx;
}
SocketContext* Session::getSocketContex()
{
	return ctx_;
}

bool Session::initBytesBuffer(int cap)
{
	bytes_buf_ = new BytesBuffer(cap);
	if (bytes_buf_ == nullptr)
	{
		LOG_ERROR("failed init bytes buffer");
		return false;
	}

	return true;
}
muggle::BytesBuffer* Session::getBytesBuffer()
{
	return bytes_buf_;
}

void Session::setDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
Dispatcher* Session::getDispatcher()
{
	return dispatcher_;
}

void Session::setDetach(bool flag)
{
	detached_ = flag;
}
bool Session::isDetached()
{
	return detached_;
}

void Session::updateActiveTime(time_t ts)
{
	last_actived_ts_ = ts;
}
time_t Session::lastActiveTime()
{
	return last_actived_ts_;
}

void Session::setAddr(const char *addr)
{
	addr_ = addr;
}
const char* Session::getAddr()
{
	return addr_.c_str();
}

bool Session::handleMessage()
{
	return dispatcher_->recvMessage(this);
}
bool Session::sendMessage(void *data, uint32_t datalen)
{
	return dispatcher_->sendMessage(this, data, datalen);
}

void Session::Close()
{
	ctx_->Shutdown();
}

NS_MUGGLE_DEMO_END
