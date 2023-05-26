#include "muggle/c/muggle_c.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"

#define KEY_SIZE 32
#define HASH_SIZE WC_SHA256_DIGEST_SIZE

typedef struct msg {
	char data[32]; //!< data
} msg_t;

int gen_key(byte *buf, uint32_t bufsize)
{
	int ret = 0;

	RNG rng;
	ret = wc_InitRng(&rng);
	if (ret != 0) {
		LOG_ERROR("failed init wolfssl RNG");
		return ret;
	}

	// salt size equal hash output size
	ret = wc_RNG_GenerateBlock(&rng, buf, bufsize);
	if (ret != 0) {
		LOG_ERROR("failed generate salt");
		return ret;
	}

	// free rng
	ret = wc_FreeRng(&rng);
	if (ret != 0) {
		LOG_ERROR("failed free RNG");
	}

	return 0;
}

byte *hmac_sign(const msg_t *p_msg, const byte *secure_key,
				const struct timespec *ts, byte *hash)
{
	memset(hash, 0, HASH_SIZE);

	Hmac hmac;
	if (wc_HmacInit(&hmac, NULL, 0) != 0) {
		LOG_ERROR("failed initialize HMAC");
		return NULL;
	}

	if (wc_HmacSetKey(&hmac, WC_SHA256, secure_key, KEY_SIZE) != 0) {
		LOG_ERROR("failed HMAC set key");
		return NULL;
	}

	if (wc_HmacUpdate(&hmac, (const byte *)p_msg, sizeof(*p_msg)) != 0) {
		LOG_ERROR("failed HMAC update message");
		return NULL;
	}

	if (wc_HmacUpdate(&hmac, (const byte *)ts, sizeof(*ts)) != 0) {
		LOG_ERROR("failed HMAC update timestamp");
		return NULL;
	}

	if (wc_HmacFinal(&hmac, hash) != 0) {
		LOG_ERROR("failed HMAC Final");
		return NULL;
	}

	wc_HmacFree(&hmac);

	return hash;
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// generate ak/sk
	byte access_key[KEY_SIZE];
	if (gen_key(access_key, sizeof(access_key)) != 0) {
		LOG_ERROR("failed generate AK");
		exit(EXIT_FAILURE);
	}

	char ak_hex[KEY_SIZE * 2 + 1];
	memset(ak_hex, 0, sizeof(ak_hex));
	muggle_hex_from_bytes((uint8_t *)access_key, ak_hex, KEY_SIZE);
	LOG_INFO("generate AK: %s", ak_hex);

	byte secure_key[KEY_SIZE];
	if (gen_key(secure_key, sizeof(secure_key)) != 0) {
		LOG_ERROR("failed generate SK");
		exit(EXIT_FAILURE);
	}

	char sk_hex[KEY_SIZE * 2 + 1];
	memset(sk_hex, 0, sizeof(sk_hex));
	muggle_hex_from_bytes((uint8_t *)secure_key, sk_hex, KEY_SIZE);
	LOG_INFO("generate SK: %s", sk_hex);

	// fillup message
	const char *data = "hello world";

	msg_t msg;
	memset(&msg, 0, sizeof(msg));
	strncpy(msg.data, data, sizeof(msg.data) - 1);

	// get timestamp
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	// hmac
	byte sign[HASH_SIZE];
	if (hmac_sign(&msg, secure_key, &ts, sign) == NULL) {
		LOG_ERROR("failed HMAC");
		exit(EXIT_FAILURE);
	}

	char sign_hex[HASH_SIZE * 2 + 1];
	memset(sign_hex, 0, sizeof(sign_hex));
	muggle_hex_from_bytes((uint8_t *)sign, sign_hex, HASH_SIZE);

	LOG_INFO("message\n{\n"
			 "\tAK: %s\n"
			 "\ttimestamp: %llu.%09u\n"
			 "\tpayload: %s\n"
			 "\tHMAC sign: %s\n"
			 "}",
			 ak_hex, (unsigned long long)ts.tv_sec, (unsigned long)ts.tv_nsec,
			 msg.data, sign_hex);

	return 0;
}
