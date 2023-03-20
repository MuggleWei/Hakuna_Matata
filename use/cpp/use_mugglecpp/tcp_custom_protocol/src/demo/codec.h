#ifndef MUGGLE_DEMO_CODEC_H_
#define MUGGLE_DEMO_CODEC_H_

#include "demo/macro.h"
#include "demo/session.h"

NS_MUGGLE_DEMO_BEGIN

class Dispatcher;
class Codec;

class Codec
{
public:
	MUGGLE_DEMO_EXPORT
	Codec();
	MUGGLE_DEMO_EXPORT
	virtual ~Codec();

	MUGGLE_DEMO_EXPORT
	virtual bool encode(Session *session, void *data, uint32_t datalen) = 0;

	MUGGLE_DEMO_EXPORT
	virtual bool decode(Session *session, void *data, uint32_t datalen) = 0;

	MUGGLE_DEMO_EXPORT
	void append(Codec *codec);

protected:
	Codec *prev_;
	Codec *next_;
};

NS_MUGGLE_DEMO_END

#endif // !MUGGLE_DEMO_CODEC_H_
