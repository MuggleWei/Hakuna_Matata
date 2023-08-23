#include "muggle/c/muggle_c.h"
#include "openssl/rand.h"
#include "openssl/hmac.h"
#include "openssl/evp.h"
#include "openssl/err.h"
// NOTE: base32.h and base32.c copy from: https://github.com/google/google-authenticator-libpam
#include "base32.h"

#define KEY_SIZE 16
#define KEY_BASE32_SIZE (KEY_SIZE * 8 + 4) / 5 + 1

uint32_t totp(time_t ts, char *key, uint32_t keylen)
{
	// convert t to big endian if system is little endian
	uint64_t x = 30;
	uint64_t t = (uint64_t)((double)ts / x);
	if (MUGGLE_ENDIANNESS == MUGGLE_LITTLE_ENDIAN) {
		t = MUGGLE_ENDIAN_SWAP_64(t);
	}

	unsigned int outlen = EVP_MD_get_size(EVP_sha1());
	unsigned char *digest = (unsigned char *)malloc(outlen);
	if (HMAC(EVP_sha1(), key, keylen, (unsigned char *)&t, sizeof(t), digest,
			 &outlen) == NULL) {
		LOG_ERROR("failed hmac: %s", ERR_reason_error_string(ERR_get_error()));
		free(digest);
		exit(EXIT_FAILURE);
	}

	// dynamically truncates hash
	uint32_t offset = digest[outlen - 1] & 0x0f;
	uint32_t result =
		(digest[offset] & 0x7f) << 24 | (digest[offset + 1] & 0xff) << 16 |
		(digest[offset + 2] & 0xff) << 8 | (digest[offset + 3] & 0xff);
	result %= 1000000;

	free(digest);

	return result;
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	unsigned char key[KEY_SIZE];
	if (argc >= 2) {
		memset(key, 0, sizeof(key));

		const char *key_base32 = argv[1];
		if (base32_decode((const unsigned char *)key_base32, key, sizeof(key)) <
			0) {
			LOG_ERROR("failed decode base32 key: %s", key_base32);
			exit(EXIT_FAILURE);
		}
	} else {
		LOG_INFO("run without input base32 key, generate random key");
		RAND_bytes(key, sizeof(key));
	}

	char key_hex[KEY_SIZE * 2 + 1];
	muggle_hex_from_bytes(key, key_hex, sizeof(key));
	key_hex[KEY_SIZE * 2] = '\0';

	unsigned char key_base32[KEY_BASE32_SIZE + 1];
	base32_encode((uint8_t *)key, sizeof(key), key_base32, sizeof(key_base32));

	// generate TOTP
	time_t ts = time(NULL);
	LOG_INFO("key (hex): %s", key_hex);
	LOG_INFO("key (base32): %s", key_base32);
	LOG_INFO("ts: %llu", (unsigned long long)ts);
	LOG_INFO("now: %06u", totp(ts, (char *)key, sizeof(key)));
	LOG_INFO("-30s: %06u", totp(ts - 30, (char *)key, sizeof(key)));
	LOG_INFO("+30s: %06u", totp(ts + 30, (char *)key, sizeof(key)));

	return 0;
}
