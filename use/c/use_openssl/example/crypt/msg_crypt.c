#include "muggle/c/muggle_c.h"
#include "openssl/evp.h"
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/core.h"

/**
 * @brief encrypt message
 *
 * @param algo_name  encrypt algorithm name
 * @param key        key
 * @param iv         iv
 * @param msg        message
 *
 * @return
 *   - 0: ok
 *   - otherwise: failed
 */
int do_crypt(
	const char *algo_name,
	int do_encrypt,
	const unsigned char *key,
	const unsigned char *iv,
	const unsigned char *in,
	int inlen,
	unsigned char *out,
	int *len)
{
	/*
     * For ciphertext stealing mode the length of the ciphertext "out" will be
     * the same size as the plaintext size "msg_len".
     * The "msg_len" can be any size >= 16.
     */
	int ret = 1;
	EVP_CIPHER_CTX *ctx = NULL;
	EVP_CIPHER *cipher = NULL;
	int outlen = 0;

	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{
		LOG_ERROR("failed create context");
		goto err;
	}

	cipher = EVP_CIPHER_fetch(NULL, algo_name, NULL);
	if (cipher == NULL)
	{
		LOG_ERROR("failed fetch algorithm");
		goto err;
	}

	if (!EVP_CipherInit_ex2(ctx, cipher, key, iv, do_encrypt, NULL))
	{
		LOG_ERROR("failed init cipher");
		goto err;
	}

	if (!EVP_CipherUpdate(ctx, out, &outlen, in, inlen))
	{
		LOG_ERROR("failed cipher update");
		goto err;
	}

	if (!EVP_CipherFinal_ex(ctx, out + outlen, len))
	{
		LOG_ERROR("failed cipher final");
		goto err;
	}

	ret = 0;

err:
	if (cipher)
	{
		EVP_CIPHER_free(cipher);
	}

	if (ctx)
	{
		EVP_CIPHER_CTX_free(ctx);
	}

	if (ret != 0)
	{
		ERR_print_errors_fp(stderr);
	}

	return ret;
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	unsigned char key[] = "0123456789abcdef";
	unsigned char iv[] = "1234567887654321";
	const char *plaintext = "hello world";
	int len = strlen(plaintext);
	int outlen = 0;
	char *ciphertext = (char*)malloc(len + EVP_MAX_BLOCK_LENGTH);
	char *rettext = (char*)malloc(len + 1);
	// Or you can use EVP_aes_128_cbc() in functions directly
	const char *algo_name = "AES-128-CBC";

	do_crypt(
		algo_name, 1, key, iv,
		(const unsigned char*)plaintext, len,
		(unsigned char*)ciphertext, &outlen);
	BIO_dump_fp(stdout, ciphertext, outlen);

	do_crypt(
		algo_name, 0, key, iv,
		(const unsigned char*)ciphertext, outlen,
		(unsigned char*)rettext, &outlen);
	BIO_dump_fp(stdout, rettext, outlen);

	free(rettext);
	free(ciphertext);

	return 0;
}
