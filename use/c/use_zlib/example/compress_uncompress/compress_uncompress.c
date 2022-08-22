#include "muggle/c/muggle_c.h"
#include "zlib.h"

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
	int err = compress(compr, &compr_len, (const Bytef*)msg, len);
	if (err != Z_OK)
	{
		LOG_ERROR("failed compress");
		exit(EXIT_FAILURE);
	}

	LOG_INFO("compress success, output bytes number: %lu", (unsigned long)compr_len);

	err = uncompress(uncompr, &uncompr_len, compr, compr_len);
	if (err != Z_OK)
	{
		LOG_ERROR("failed compress");
		exit(EXIT_FAILURE);
	}

	LOG_INFO("uncompress result: uncomprLen=%lu",
		(unsigned long)uncompr_len);

	MUGGLE_ASSERT(len == uncompr_len);
	MUGGLE_ASSERT(strncmp(msg, (char*)uncompr, strlen(msg)) == 0);

	// free resources
	free(compr);
	free(uncompr);
	
	return 0;
}
