#include "dispatcher.h"
#include "muggle/cpp/muggle_cpp.h"
#include "demo/msg.h"
#include "demo/codec.h"
#include "demo/session.h"

NS_MUGGLE_DEMO_BEGIN

Dispatcher::Dispatcher()
	: total_msg_(0)
	, callbacks_(nullptr)
	, codec_list_head_(nullptr)
	, codec_list_tail_(nullptr)
{}
Dispatcher::~Dispatcher()
{
	if (callbacks_)
	{
		free(callbacks_);
		callbacks_ = nullptr;
	}
}

bool Dispatcher::setTotalMessage(uint32_t total_msg)
{
	if (total_msg_ != 0)
	{
		LOG_WARNING("failed to repeated set dispatcher total message");
		return false;
	}

	callbacks_ = (NetCallbackFunc*)malloc(sizeof(NetCallbackFunc) * total_msg);
	if (callbacks_ == nullptr)
	{
		LOG_ERROR("failed set dispatcher total message");
		return false;
	}
	total_msg_ = total_msg;

	for (uint32_t i = 0; i < total_msg_; i++)
	{
		callbacks_[i] = nullptr;
	}

	return true;
}

uint32_t Dispatcher::getTotalMessage()
{
	return total_msg_;
}

bool Dispatcher::registerCallback(uint32_t msg_id, NetCallbackFunc cb)
{
	if (msg_id >= total_msg_)
	{
		LOG_ERROR("failed register callback function, message id beyond max: "
			"msg_id=%lu, total_msg=%lu",
			(unsigned long)msg_id, (unsigned long)total_msg_);
		return false;
	}

	NetCallbackFunc callback = callbacks_[msg_id];
	if (callback != nullptr)
	{
		LOG_ERROR("failed register callback function, "
			"message already register: msg_id=%lu",
			(unsigned long)msg_id);
		return false;
	}

	callbacks_[msg_id] = cb;

	return true;
}

void Dispatcher::appendCodec(Codec *codec)
{
	if (codec_list_tail_ == nullptr)
	{
		codec_list_head_ = codec;
	}
	else
	{
		codec_list_tail_->append(codec);
	}
	codec_list_tail_ = codec;
}

bool Dispatcher::encode(Codec *codec,
	Session *session,
	void *data, uint32_t datalen)
{
	if (codec)
	{
		return codec->encode(session, data, datalen);
	}
	else
	{
		SocketContext *ctx = session->getSocketContex();
		if (ctx == nullptr)
		{
			MUGGLE_ASSERT(ctx != nullptr);
			LOG_ERROR("failed get socket socket, addr=%s", session->getAddr());
			return false;
		}

		msg_hdr_t *hdr = (msg_hdr_t*)data;
		LOG_TRACE("dispatcher send message, addr=%s, msg_id=%lu",
			session->getAddr(), (unsigned long)hdr->msg_id);

		int n = ctx->Write(data, datalen);
		if (n != (int)datalen)
		{
			if (n == MUGGLE_EVENT_ERROR)
			{
				char errmsg[512];
				snprintf(errmsg, sizeof(errmsg), "failed send message, "
					"addr=%s, msg_id=%lu",
					session->getAddr(), (unsigned long)hdr->msg_id);
				MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, errmsg);
			}

			return false;
		}
	}

	return true;
}

bool Dispatcher::decode(Codec *codec,
	Session *session,
	void *data, uint32_t datalen)
{
	if (codec)
	{
		return codec->decode(session, data, datalen);
	}
	else
	{
		SocketContext *ctx = session->getSocketContex();
		if (ctx == nullptr)
		{
			MUGGLE_ASSERT(ctx != nullptr);
			LOG_ERROR("failed get socket socket, addr=%s", session->getAddr());
			return false;
		}

		msg_hdr_t *hdr = (msg_hdr_t*)data;
		LOG_TRACE("dispatcher recv message, addr=%s, msg_id=%lu",
			session->getAddr(), (unsigned long)hdr->msg_id);

		NetCallbackFunc cb_func = nullptr;
		if (hdr->msg_id < total_msg_)
		{
			cb_func = callbacks_[hdr->msg_id];
		}

		if (cb_func)
		{
			cb_func(session, data);
		}
		else
		{
			LOG_WARNING("failed find callback, addr=%s, msg_id=%lu",
				session->getAddr(), (unsigned long)hdr->msg_id);
		}
	}

	return true;
}

bool Dispatcher::recvMessage(Session *session)
{
	Codec *codec = codec_list_tail_;
	return decode(codec, session, nullptr, 0);
}

bool Dispatcher::sendMessage(Session *session, void *data, uint32_t datalen)
{
	Codec *codec = codec_list_head_;
	return encode(codec, session, data, datalen);
}

NS_MUGGLE_DEMO_END
