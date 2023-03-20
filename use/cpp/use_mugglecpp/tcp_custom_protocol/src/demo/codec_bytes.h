#ifndef MUGGLE_DEMO_CODEC_BYTES_H_
#define MUGGLE_DEMO_CODEC_BYTES_H_

#include "demo/macro.h"
#include "demo/codec.h"

NS_MUGGLE_DEMO_BEGIN

class CodecBytes : public Codec
{
public:
	MUGGLE_DEMO_EXPORT
	CodecBytes();
	MUGGLE_DEMO_EXPORT
	virtual ~CodecBytes();

	MUGGLE_DEMO_EXPORT
	virtual bool encode(
		Session *session, void *data, uint32_t datalen) override;

	MUGGLE_DEMO_EXPORT
	virtual bool decode(
		Session *session, void *data, uint32_t datalen) override;

	MUGGLE_DEMO_EXPORT
	void setRecvUnitSize(uint32_t recv_unit_size);
	MUGGLE_DEMO_EXPORT
	void setMsgLenLimit(uint32_t msg_len_limit);

private:
	uint32_t recv_unit_size_;
	uint32_t msg_len_limit_;
};

NS_MUGGLE_DEMO_END

#endif // !MUGGLE_DEMO_CODEC_BYTES_H_
