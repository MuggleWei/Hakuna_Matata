#include "muggle/c/muggle_c.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"

#define PASSWD_MAX_LEN 16
#define SALT_SIZE 32
#define SHA256_SIZE 32

int gen_salt(byte *buf, uint32_t bufsize)
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

int gen_hash(const char *passwd, uint32_t passwd_len, byte *salt, byte *hash)
{
	if (passwd_len > PASSWD_MAX_LEN) {
		LOG_ERROR("passwd length too long");
		return -1;
	}

	byte buf[PASSWD_MAX_LEN + SALT_SIZE];
	memset(buf, 0, sizeof(buf));
	memcpy(buf, passwd, passwd_len);
	memcpy(buf + passwd_len, salt, SALT_SIZE);

	int ret = wc_Sha256Hash(buf, passwd_len + SALT_SIZE, hash);
	if (ret != 0) {
		LOG_ERROR("failed generate hash");
		return ret;
	}

	return 0;
}

int passwd_hash(const char *passwd, char *salt_hex, char *hash_hex)
{
	uint32_t passwd_len = (uint32_t)strlen(passwd);
	if (passwd_len < 8 || passwd_len > PASSWD_MAX_LEN) {
		LOG_ERROR("passwd length must belong to [8, 16]");
		return -1;
	}

	// generate salt
	byte salt[SALT_SIZE];
	if (gen_salt(salt, (uint32_t)sizeof(salt)) != 0) {
		LOG_ERROR("failed generate salt");
		return -1;
	}

	// generate hash
	byte hash[WC_SHA256_DIGEST_SIZE];
	if (gen_hash(passwd, passwd_len, salt, hash) != 0) {
		LOG_ERROR("failed generate hash");
		return -1;
	}

	muggle_hex_from_bytes((uint8_t *)salt, salt_hex, SALT_SIZE);
	muggle_hex_from_bytes((uint8_t *)hash, hash_hex, WC_SHA256_DIGEST_SIZE);

	return 0;
}

bool verify_hash(const char *passwd, const char *salt_hex, const char *hash_hex)
{
	uint32_t passwd_len = (uint32_t)strlen(passwd);
	if (passwd_len < 8 || passwd_len > PASSWD_MAX_LEN) {
		LOG_ERROR("passwd length must belong to [8, 16]");
		return -1;
	}

	byte salt[SALT_SIZE];
	muggle_hex_to_bytes(salt_hex, (uint8_t *)salt, strlen(salt_hex));

	byte hash[WC_SHA256_DIGEST_SIZE];
	if (gen_hash(passwd, passwd_len, salt, hash) != 0) {
		LOG_ERROR("failed generate hash in verify hash");
		return false;
	}

	char verify_hash_hex[WC_SHA256_DIGEST_SIZE * 2 + 1];
	memset(verify_hash_hex, 0, sizeof(verify_hash_hex));
	muggle_hex_from_bytes((uint8_t *)hash, verify_hash_hex,
						  WC_SHA256_DIGEST_SIZE);

	return strncmp(verify_hash_hex, hash_hex, WC_SHA256_DIGEST_SIZE * 2) == 0;
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc != 2) {
		LOG_ERROR("Usage: %s passwd", argv[0]);
		exit(EXIT_FAILURE);
	}

	// check passwd
	const char *passwd = argv[1];

	// password hash
	char salt_hex[SALT_SIZE * 2 + 1];
	char hash_hex[WC_SHA256_DIGEST_SIZE * 2 + 1];
	memset(salt_hex, 0, sizeof(salt_hex));
	memset(hash_hex, 0, sizeof(hash_hex));

	if (passwd_hash(passwd, salt_hex, hash_hex) != 0) {
		LOG_ERROR("failed password hash");
		exit(EXIT_FAILURE);
	}
	LOG_INFO("\n----------------------\n"
			 "Generate Password Hash: \n"
			 "password: %s\n"
			 "salt: %s\n"
			 "hash: %s\n"
			 "----------------------",
			 passwd, salt_hex, hash_hex);

	// verify hash
	if (verify_hash(passwd, salt_hex, hash_hex)) {
		LOG_INFO("\n----------------------\n"
				 "Verify Hash\n"
				 "Success\n"
				 "----------------------");
	} else {
		LOG_ERROR("\n----------------------\n"
				  "Verify Hash\n"
				  "Failed\n"
				  "----------------------");
		exit(EXIT_FAILURE);
	}

	return 0;
}
