#include "codec_bytes.h"
#include "muggle/cpp/muggle_cpp.h"
#include "demo/msg.h"
#include "demo/dispatcher.h"

NS_MUGGLE_DEMO_BEGIN
	
CodecBytes::CodecBytes()
	: recv_unit_size_(4 * 1024)
	, msg_len_limit_(32 * 1024)
{}
CodecBytes::~CodecBytes()
{}

static const char *s_magic = MUGGLE_DEMO_MAGIC_WORD;
static const uint8_t s_protocol_ver = (uint8_t)1;
static const int16_t s_endianess_val = 1;
static const char s_endianess = 
	(char)(*(char*)&s_endianess_val == 1 ?
		MUGGLE_LITTLE_ENDIAN : MUGGLE_BIG_ENDIAN);
static const uint8_t s_ver = 1;

bool CodecBytes::encode(
	Session *session, void *data, uint32_t datalen)
{
	msg_hdr_t *hdr = (msg_hdr_t*)data;
	memcpy(hdr->magic, s_magic, 4);
	hdr->flags[MUGGLE_DEMO_FLAG_ENDIAN] = s_endianess;
	hdr->flags[MUGGLE_DEMO_FLAG_VER] = s_protocol_ver;
	hdr->flags[MUGGLE_DEMO_FLAG_COMPRESSED] = s_endianess;
	hdr->payload_len = datalen - sizeof(msg_hdr_t);

	Dispatcher *dispatcher = session->getDispatcher();
	return dispatcher->encode(next_, session, data, datalen);
}

bool CodecBytes::decode(
	Session *session, void *data, uint32_t datalen)
{
	MUGGLE_UNUSED(data);
	MUGGLE_UNUSED(datalen);

	BytesBuffer *bytes_buf = session->getBytesBuffer();
	if (bytes_buf == nullptr)
	{
		MUGGLE_ASSERT(bytes_buf != nullptr);
		LOG_ERROR("failed get user bytes buffer, addr=%s", session->getAddr());
		return false;
	}

	SocketContext *ctx = session->getSocketContex();
	if (ctx == nullptr)
	{
		MUGGLE_ASSERT(ctx != nullptr);
		LOG_ERROR("failed get socket context, addr=%s", session->getAddr());
		return false;
	}

	Dispatcher *dispatcher = session->getDispatcher();
	if (dispatcher == nullptr)
	{
		MUGGLE_ASSERT(dispatcher != nullptr);
		LOG_ERROR("failed get dispatcher, addr=%s", session->getAddr());
		return false;
	}

	// read bytes into bytes buffer
	while (true)
	{
		void *p = bytes_buf->WriterFc((int)recv_unit_size_);
		if (p == nullptr)
		{
			LOG_WARNING("bytes buffer full: %s", session->getAddr());
			break;
		}

		int n = ctx->Read(p, (size_t)recv_unit_size_);
		if (n > 0)
		{
			bytes_buf->WriterMove(n);
		}

		if (n < (int)recv_unit_size_)
		{
			break;
		}
	}

	// parse message
	msg_hdr_t msg_header;
	while (true)
	{
		if (!bytes_buf->Fetch((int)sizeof(msg_header), &msg_header))
		{
			// readable bytes less than sizeof message header
			break;
		}

		// check endianness
		if (msg_header.flags[MUGGLE_DEMO_FLAG_ENDIAN] != s_endianess)
		{
			LOG_ERROR("invalid endianness: addr=%s", session->getAddr());
			return false;
		}

		// check version
		if (msg_header.flags[MUGGLE_DEMO_FLAG_VER] != s_ver)
		{
			LOG_ERROR("invalid version: addr=%s", session->getAddr());
			return false;
		}

		// check compressed
		if (msg_header.flags[MUGGLE_DEMO_FLAG_COMPRESSED] != 0)
		{
			LOG_ERROR("currently not supported compressed: addr=%s",
				session->getAddr());
			return false;
		}

		// check reversed bytes
		if (msg_header.flags[MUGGLE_DEMO_FLAG_REVERSED1] != '\0')
		{
			LOG_ERROR("reversed bytes be set: addr=%s", session->getAddr());
			return false;
		}

		// check message length
		uint32_t total_bytes =
			(uint32_t)sizeof(msg_hdr_t) + msg_header.payload_len;
		if (msg_len_limit_ > 0)
		{
			if (total_bytes > msg_len_limit_)
			{
				LOG_ERROR("invalid message length: "
					"addr=%s, msg_id=%lu, msg_total_len=%lu, msg_len_limit=%lu",
					session->getAddr(), (unsigned long)msg_header.msg_id,
					(unsigned long)total_bytes, (unsigned long)msg_len_limit_);
				return false;
			}
		}

		// check readable
		int readable = bytes_buf->Readable();
		if ((uint32_t)readable < total_bytes)
		{
			break;
		}

		// handle message
		msg_hdr_t *p_msg = (msg_hdr_t*)bytes_buf->ReaderFc((int)total_bytes);
		if (p_msg == nullptr)
		{
			// discontinuous memory
			void *buf = malloc(total_bytes);
			bytes_buf->Read((int)total_bytes, buf);

			bool ret = dispatcher->decode(prev_, session, buf, total_bytes);

			free(buf);

			if (!ret)
			{
				return false;
			}
		}
		else
		{
			// continuous memory
			bool ret = dispatcher->decode(prev_, session, p_msg, total_bytes);

			bytes_buf->ReaderMove((int)total_bytes);

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

void CodecBytes::setRecvUnitSize(uint32_t recv_unit_size)
{
	recv_unit_size_ = recv_unit_size;
}

void CodecBytes::setMsgLenLimit(uint32_t msg_len_limit)
{
	msg_len_limit_ = msg_len_limit;
}

NS_MUGGLE_DEMO_END
