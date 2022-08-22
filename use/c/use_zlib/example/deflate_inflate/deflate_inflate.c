#include "muggle/c/muggle_c.h"
#include "zlib.h"

void* myalloc(void *q, unsigned int n, unsigned int m)
{
	(void)q;
	return malloc(n * m);
}

void myfree(void *q, void *p)
{
	(void)q;
	free(p);
}

Byte* do_compress(const char *msg, uLong len, Byte *compr, uLong *compr_len)
{
	// init z_stream
	z_stream strm;
	memset(&strm, 0, sizeof(strm));

	strm.zalloc = myalloc;
	strm.zfree = myfree;
	int res = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	if (res != Z_OK)
	{
		LOG_ERROR("failed deflateInit");
		return NULL;
	}

	// compress
	strm.next_in = (Byte*)msg;
	strm.avail_in = len;
	strm.next_out = compr;
	strm.avail_out = *compr_len;

	res = deflate(&strm, Z_NO_FLUSH);
	if (res != Z_OK)
	{
		LOG_ERROR("failed deflate");
		deflateEnd(&strm);
		return NULL;
	}

	if (strm.avail_in != 0)
	{
		LOG_ERROR("failed deflate, avail_in != 0");
		deflateEnd(&strm);
		return NULL;
	}

	res = deflate(&strm, Z_FINISH);
	if (res != Z_STREAM_END)
	{
		LOG_ERROR("failed deflate finish");
		res = deflateEnd(&strm);
		return NULL;
	}

	*compr_len = strm.total_out;

	// free z_stream
	res = deflateEnd(&strm);
	if (res != Z_OK)
	{
		LOG_ERROR("failed deflateEnd");
	}

	return compr;
}

Byte* do_uncompress(Byte *compr, uLong compr_len, Byte *uncompr, uLong *uncompr_len)
{
	// init z_stream
	z_stream strm;
	memset(&strm, 0, sizeof(strm));

	strm.zalloc = myalloc;
	strm.zfree = myfree;

	strm.next_in = compr;
	strm.avail_in = compr_len;

	int res = inflateInit(&strm);
	if (res != Z_OK)
	{
		LOG_ERROR("failed deflateInit");
		return NULL;
	}

	strm.next_out = uncompr;
	strm.avail_out = *uncompr_len;
	res = inflate(&strm, Z_NO_FLUSH);
	if (res != Z_STREAM_END)
	{
		LOG_ERROR("large inflate");
		inflateEnd(&strm);
		return NULL;
	}

	res = inflateEnd(&strm);
	if (res != Z_OK)
	{
		LOG_ERROR("failed inflateEnd");
	}

	*uncompr_len = strm.total_out;

	return uncompr;
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// prepare message
	const char *msg =
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789"
		")!@#$%^&*("
		"ABABABABABABABABABABABABAB"
		"BBBBBBBBBBBBBBBBBBBBBBBBBB"
		"CCCCCCCCCCCCCCCCCCCCCCCCCC"
		"DDDDDDDDDDDDDDDDDDDDDDDDDD";
	uLong len = (uLong)strlen(msg);

	// prepare compress/uncompress buffer
	uLong compr_len = 1024 * 16;
	uLong uncompr_len = compr_len;

	Byte *compr = (Byte*)malloc(compr_len);
	Byte *uncompr = (Byte*)malloc(uncompr_len);

	// do compress/uncompress
	if (do_compress(msg, len, compr, &compr_len) == NULL)
	{
		LOG_ERROR("failed do compress");
		exit(EXIT_FAILURE);
	}

	LOG_INFO("deflate output number of bytes: %lu", (unsigned long)compr_len);

	if (do_uncompress(compr, compr_len, uncompr, &uncompr_len) == NULL)
	{
		LOG_ERROR("failed do uncompress");
		exit(EXIT_FAILURE);
	}

	LOG_INFO("inflate output number of bytes: %lu", (unsigned long)uncompr_len);

	MUGGLE_ASSERT(len == uncompr_len);
	MUGGLE_ASSERT(strncmp(msg, (char*)uncompr, strlen(msg)) == 0);

	// free resources
	free(compr);
	free(uncompr);

	return 0;
}
