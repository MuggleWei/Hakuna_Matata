#include "muggle/c/muggle_c.h"
#include "openssl/evp.h"
#include "openssl/bio.h"
#include "openssl/err.h"

/**
 * @brief output message digest
 *
 * @param msg        message
 * @param msglen     length of message
 * @param algo_name  algorithm name
 *
 * @return
 *   - 0: ok
 *   - otherwise: failed
 */
int output_digest(const char *msg, size_t msglen, const char *algo_name)
{
	int ret = 1;
	EVP_MD_CTX *ctx = NULL;
	EVP_MD *algo = NULL;
	unsigned char *outdigest = NULL;
	unsigned int len = 0;

	// create a context
	ctx = EVP_MD_CTX_new();
	if (ctx == NULL)
	{
		LOG_ERROR("failed create context");
		goto err;
	}

	algo = EVP_MD_fetch(NULL, algo_name, NULL);
	if (algo == NULL)
	{
		LOG_ERROR("failed fetch algorithm");
		goto err;
	}

	// initialise the digest operation
	if (!EVP_DigestInit_ex(ctx, algo, NULL))
	{
		LOG_ERROR("failedc init digest");
		goto err;
	}

	// pass message to be digested.
	if (!EVP_DigestUpdate(ctx, msg, strlen(msg)))
	{
		LOG_ERROR("failed digest update");
		goto err;
	}

	outdigest = OPENSSL_malloc(EVP_MD_get_size(algo));
	if (outdigest == NULL)
	{
		LOG_ERROR("failed allocated output buffer");
		goto err;
	}

	if (!EVP_DigestFinal_ex(ctx, outdigest, &len))
	{
		LOG_ERROR("failed digest final");
		goto err;
	}

	BIO_dump_fp(stdout, outdigest, len);

	ret = 0;
err:

	if (outdigest)
	{
		OPENSSL_free(outdigest);
	}

	if (algo)
	{
		EVP_MD_free(algo);
	}

	if (ctx)
	{
		EVP_MD_CTX_free(ctx);
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

	if (argc < 3)
	{
		LOG_ERROR(
			"--------------------\n"
			"Usage: %s <algorithm> <message>\n"
			"e.g.\n"
			"\t%s SHA256 \"hello world\"",
			argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	LOG_INFO("digest[%s] %s", argv[1], argv[2]);

	const char *algo_name = argv[1];
	const char *msg = argv[2];
	output_digest(msg, strlen(msg), algo_name);

	return 0;
}
