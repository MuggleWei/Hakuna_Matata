#ifndef MUGGLE_DEMO_DISPATCHER_H_
#define MUGGLE_DEMO_DISPATCHER_H_

#include <vector>
#include "demo/macro.h"
#include "demo/session.h"

NS_MUGGLE_DEMO_BEGIN

class Codec;

typedef void (*NetCallbackFunc)(Session *session, void *msg);

class Dispatcher
{
public:
	MUGGLE_DEMO_EXPORT
	Dispatcher();

	MUGGLE_DEMO_EXPORT
	virtual ~Dispatcher();

	MUGGLE_DEMO_EXPORT
	bool setTotalMessage(uint32_t total_msg);

	MUGGLE_DEMO_EXPORT
	uint32_t getTotalMessage();

	MUGGLE_DEMO_EXPORT
	bool registerCallback(uint32_t msg_id, NetCallbackFunc cb);

	MUGGLE_DEMO_EXPORT
	void appendCodec(Codec *codec);

	MUGGLE_DEMO_EXPORT
	bool encode(Codec *codec,
		Session *session,
		void *data, uint32_t datalen);

	MUGGLE_DEMO_EXPORT
	bool decode(Codec *codec,
		Session *session,
		void *data, uint32_t datalen);

	MUGGLE_DEMO_EXPORT
	bool recvMessage(Session *session);

	MUGGLE_DEMO_EXPORT
	bool sendMessage(Session *session, void *data, uint32_t datalen);

private:
	uint32_t total_msg_;
	NetCallbackFunc *callbacks_;

	Codec *codec_list_head_;
	Codec *codec_list_tail_;
};

NS_MUGGLE_DEMO_END

#endif // !MUGGLE_DEMO_DISPATCHER_H_
