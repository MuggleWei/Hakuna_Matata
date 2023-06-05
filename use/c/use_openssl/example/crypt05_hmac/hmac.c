#include <stdlib.h>
#include <string.h>
#include "muggle/c/encoding/hex.h"
#include "muggle/c/log/log.h"
#include "muggle/c/os/path.h"
#include "muggle/c/version/version.h"
#include "openssl/rand.h"
#include "openssl/hmac.h"
#include "openssl/evp.h"
#include "openssl/err.h"

#define KEY_SIZE 32

struct my_message {
	uint64_t idx; //!< message index
	struct timespec ts; //!< timestamp
	char data[32]; //!< data
};

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// generate ak/sk
	unsigned char access_key[KEY_SIZE];
	RAND_bytes(access_key, sizeof(access_key));

	unsigned char secure_key[KEY_SIZE];
	RAND_bytes(secure_key, sizeof(secure_key));

	char ak_hex[KEY_SIZE * 2 + 1];
	muggle_hex_from_bytes(access_key, ak_hex, sizeof(access_key));
	ak_hex[KEY_SIZE * 2] = '\0';

	char sk_hex[KEY_SIZE * 2 + 1];
	muggle_hex_from_bytes(secure_key, sk_hex, sizeof(secure_key));
	sk_hex[KEY_SIZE * 2] = '\0';

	LOG_INFO("generate AK: %s", ak_hex);
	LOG_INFO("generate SK: %s", sk_hex);

	// fillup message
	struct my_message msg;
	memset(&msg, 0, sizeof(msg));
	msg.idx = 1;
	strncpy(msg.data, "hello world", sizeof(msg.data) - 1);
	timespec_get(&msg.ts, TIME_UTC);

	// write message into hmac_msg.txt, for command line compare
	FILE *fp = fopen("hmac_msg.txt", "wb");
	if (fp == NULL) {
		LOG_ERROR("failed open hmac_msg.txt");
		exit(EXIT_FAILURE);
	}
	fwrite(&msg, 1, sizeof(msg), fp);
	fclose(fp);

	// hmac
	unsigned int outlen = EVP_MD_get_size(EVP_sha256());
	unsigned char *md = (unsigned char *)malloc(outlen);
	if (HMAC(EVP_sha256(), sk_hex, strlen(sk_hex), (unsigned char *)&msg,
			 sizeof(msg), md, &outlen) == NULL) {
		LOG_ERROR("failed hmac: %s", ERR_reason_error_string(ERR_get_error()));
		free(md);
		exit(EXIT_FAILURE);
	}

	char *output_hex = (char *)malloc(outlen * 2 + 1);
	muggle_hex_from_bytes(md, output_hex, outlen);
	output_hex[outlen * 2] = '\0';
	LOG_INFO("hmac message\n"
			 "  <=> openssl dgst -sha256 -hmac %s -hex hmac_msg.txt\n"
			 "output: %s",
			 sk_hex, output_hex);

	free(output_hex);
	free(md);

	return 0;
}
