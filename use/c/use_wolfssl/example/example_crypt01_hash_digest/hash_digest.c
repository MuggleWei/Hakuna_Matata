#include <stdlib.h>
#include "muggle/c/log/log.h"
#include "muggle/c/encoding/hex.h"
#include "wolfssl/ssl.h"
#include "wolfssl/wolfcrypt/types.h"
#include "wolfssl/wolfcrypt/md5.h"
#include "wolfssl/wolfcrypt/sha256.h"

void md5sum(const char *data)
{
	int ret = 0;

	Md5 md5;
	ret = wc_InitMd5(&md5);
	if (ret != 0) {
		LOG_ERROR("wc_Initmd5 failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	ret = wc_Md5Update(&md5, (const unsigned char *)data, strlen(data));
	if (ret != 0) {
		LOG_ERROR("wc_Md5Update failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	byte hash[MD5_DIGEST_SIZE];
	ret = wc_Md5Final(&md5, hash);
	if (ret != 0) {
		LOG_ERROR("wc_Md5Final failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	char hash_hex[MD5_DIGEST_SIZE * 2 + 1];
	muggle_hex_from_bytes(hash, hash_hex, MD5_DIGEST_SIZE);
	hash_hex[MD5_DIGEST_SIZE * 2] = '\0';
	LOG_INFO("MD5: %s", hash_hex);
}

void sha256sum(const char *data)
{
	int ret = 0;

	Sha256 sha;
	ret = wc_InitSha256(&sha);
	if (ret != 0) {
		LOG_ERROR("wc_InitSha256 failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	ret = wc_Sha256Update(&sha, (const unsigned char *)data, strlen(data));
	if (ret != 0) {
		LOG_ERROR("wc_Sha256Update failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	byte hash[SHA256_DIGEST_SIZE];
	ret = wc_Sha256Final(&sha, hash);
	if (ret != 0) {
		LOG_ERROR("wc_Sha256Final failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	char hash_hex[SHA256_DIGEST_SIZE * 2 + 1];
	muggle_hex_from_bytes(hash, hash_hex, SHA256_DIGEST_SIZE);
	hash_hex[SHA256_DIGEST_SIZE * 2] = '\0';
	LOG_INFO("SHA256: %s", hash_hex);
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc < 2) {
		LOG_ERROR("Usage: %s <message>", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char *data = argv[1];

	LOG_INFO("input message: %s", data);

	md5sum(data);
	sha256sum(data);

	return 0;
}
