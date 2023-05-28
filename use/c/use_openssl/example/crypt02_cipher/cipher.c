#include <string.h>
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/evp.h"
#include "muggle/c/encoding/hex.h"
#include "muggle/c/log/log.h"
#include "muggle/c/version/version.h"

bool cipher_encrypt(const char *algorithm, const char *plaintext,
					unsigned int plaintext_len, const unsigned char *key,
					const unsigned char *iv, int key_size, int iv_size,
					int cipher_block_size, unsigned char *outbuf,
					unsigned int *outbuf_len)
{
	bool ret = false;

	EVP_CIPHER_CTX *ctx = NULL;
	EVP_CIPHER *algo = NULL;

	// create a context for the cipher operation
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("EVP_CIPHER_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}

	// fetch the cipher algorithm implementation
	algo = EVP_CIPHER_fetch(NULL, algorithm, NULL);
	if (algo == NULL) {
		LOG_ERROR("EVP_CIPHER_fetch failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}

	if (EVP_CipherInit_ex2(ctx, algo, key, iv, 1, NULL) == 0) {
		LOG_ERROR("EVP_CipherInit_ex2 failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}

	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == key_size);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == iv_size);
	OPENSSL_assert(EVP_CIPHER_CTX_block_size(ctx) == cipher_block_size);

	int outlen = 0;
	*outbuf_len = 0;
	if (!EVP_CipherUpdate(ctx, outbuf, &outlen,
						  (const unsigned char *)plaintext, plaintext_len)) {
		LOG_ERROR("EVP_CipherUpdate failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}
	*outbuf_len += outlen;

	if (!EVP_CipherFinal_ex(ctx, outbuf + *outbuf_len, &outlen)) {
		LOG_ERROR("EVP_CipherFinal_ex failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}
	*outbuf_len += outlen;

	char *hex = (char *)malloc(*outbuf_len * 2 + 1);
	muggle_hex_from_bytes(outbuf, hex, *outbuf_len);
	hex[*outbuf_len * 2] = '\0';
	LOG_INFO("%s: %s", algorithm, hex);
	free(hex);

	ret = true;

cipher_exit:
	if (algo) {
		EVP_CIPHER_free(algo);
	}

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
	}

	return ret;
}

bool cipher_decrypt(const char *algorithm, const unsigned char *ciphertext,
					unsigned int ciphertext_len, const unsigned char *key,
					const unsigned char *iv, int key_size, int iv_size,
					int cipher_block_size, unsigned char *outbuf)
{
	bool ret = false;

	EVP_CIPHER_CTX *ctx = NULL;
	EVP_CIPHER *algo = NULL;

	// create a context for the cipher operation
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("EVP_CIPHER_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}

	// fetch the cipher algorithm implementation
	algo = EVP_CIPHER_fetch(NULL, algorithm, NULL);
	if (algo == NULL) {
		LOG_ERROR("EVP_CIPHER_fetch failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}

	if (EVP_CipherInit_ex2(ctx, algo, key, iv, 0, NULL) == 0) {
		LOG_ERROR("EVP_CipherInit_ex2 failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}

	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == key_size);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == iv_size);
	OPENSSL_assert(EVP_CIPHER_CTX_block_size(ctx) == cipher_block_size);

	int outlen = 0;
	int outbuf_len = 0;
	if (!EVP_CipherUpdate(ctx, outbuf, &outlen,
						  (const unsigned char *)ciphertext, ciphertext_len)) {
		LOG_ERROR("EVP_CipherUpdate failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}
	outbuf_len += outlen;

	if (!EVP_CipherFinal_ex(ctx, outbuf + outbuf_len, &outlen)) {
		LOG_ERROR("EVP_CipherFinal_ex failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto cipher_exit;
	}
	outbuf_len += outlen;

	LOG_INFO("decrypt get plaintext: %.*s", outbuf_len, outbuf);

	ret = true;

cipher_exit:
	if (algo) {
		EVP_CIPHER_free(algo);
	}

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
	}

	return ret;
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc < 2) {
		LOG_ERROR("Usage: %s <message>", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char *plaintext = argv[1];

	LOG_INFO("mugglec version: %s", mugglec_version());
	LOG_INFO("mugglec compile time: %s", mugglec_compile_time());
	LOG_INFO("OpenSSL version: %s", OpenSSL_version(OPENSSL_VERSION));

	size_t len = strlen(plaintext);
	unsigned int ciphertext_len = len + EVP_MAX_BLOCK_LENGTH;
	unsigned char *ciphertext = (unsigned char *)malloc(ciphertext_len);
	unsigned char *ret_plaintext = (unsigned char *)malloc(ciphertext_len);

	LOG_INFO("input message: %s", plaintext);

	LOG_INFO("-------- AES-128-CBC --------");

	ciphertext_len = 0;
	const int AES_128_KEY_SIZE = 16;
	const int AES_128_IV_SIZE = 16;
	const int AES_128_BLOCK_SIZE = 16;
	unsigned char aes128_key[] = "0123456789abcdef";
	unsigned char aes128_iv[] = "1234567887654321";
	LOG_INFO("AES-128-CBC key: %.*s", AES_128_KEY_SIZE, aes128_key);
	LOG_INFO("AES-128-CBC iv: %.*s", AES_128_IV_SIZE, aes128_iv);
	cipher_encrypt("AES-128-CBC", plaintext, strlen(plaintext), aes128_key,
				   aes128_iv, AES_128_KEY_SIZE, AES_128_IV_SIZE,
				   AES_128_BLOCK_SIZE, ciphertext, &ciphertext_len);
	cipher_decrypt("AES-128-CBC", ciphertext, ciphertext_len, aes128_key,
				   aes128_iv, AES_128_KEY_SIZE, AES_128_IV_SIZE,
				   AES_128_BLOCK_SIZE, ret_plaintext);

	LOG_INFO("-------- AES-256-CBC --------");

	ciphertext_len = 0;
	const int AES_256_KEY_SIZE = 32;
	const int AES_256_IV_SIZE = 16;
	const int AES_256_BLOCK_SIZE = 16;
	unsigned char aes256_key[] = "0123456789abcdeffedcba9876543210";
	unsigned char aes256_iv[] = "1234567887654321";
	LOG_INFO("AES-256-CBC key: %.*s", AES_256_KEY_SIZE, aes256_key);
	LOG_INFO("AES-256-CBC iv: %.*s", AES_256_IV_SIZE, aes256_iv);
	cipher_encrypt("AES-256-CBC", plaintext, strlen(plaintext), aes256_key,
				   aes256_iv, AES_256_KEY_SIZE, AES_256_IV_SIZE,
				   AES_256_BLOCK_SIZE, ciphertext, &ciphertext_len);
	cipher_decrypt("AES-256-CBC", ciphertext, ciphertext_len, aes256_key,
				   aes256_iv, AES_256_KEY_SIZE, AES_256_IV_SIZE,
				   AES_256_BLOCK_SIZE, ret_plaintext);

	free(ret_plaintext);
	free(ciphertext);

	return 0;
}
