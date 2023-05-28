#include <string.h>
#include "muggle/c/encoding/hex.h"
#include "muggle/c/log/log.h"
#include "muggle/c/version/version.h"
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/evp.h"

bool digest(const char *data, const char *algorithm)
{
	bool ret = false;
	EVP_MD_CTX *ctx = NULL;
	EVP_MD *algo = NULL;
	unsigned char *outdigest = NULL;

	// create a context for the digest operation
	ctx = EVP_MD_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("EVP_MD_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	// fetch the digest algorithm implementation
	algo = EVP_MD_fetch(NULL, algorithm, NULL);
	if (algo == NULL) {
		LOG_ERROR("EVP_MD_fetch failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	// initialize the disgest operation
	if (!EVP_DigestInit_ex(ctx, algo, NULL)) {
		LOG_ERROR("EVP_DigestInit_ex failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	// pass the message to be digested
	if (!EVP_DigestUpdate(ctx, data, strlen(data))) {
		LOG_ERROR("EVP_DigestUpdate failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	// allocate the output buffer
	outdigest = OPENSSL_malloc(EVP_MD_get_size(algo));
	if (outdigest == NULL) {
		LOG_ERROR("OPENSSL_malloc failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	// calculate the digest itself
	unsigned int len = 0;
	if (!EVP_DigestFinal(ctx, outdigest, &len)) {
		LOG_ERROR("EVP_DigestFinal failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	char *hex = (char*)malloc(len * 2 + 1);
	muggle_hex_from_bytes(outdigest, hex, len);
	hex[len * 2] = '\0';
	LOG_INFO("%s: %s", algorithm, hex);
	free(hex);

	ret = true;

digest_exit:
	if (outdigest) {
		OPENSSL_free(outdigest);
	}

	if (algo) {
		EVP_MD_free(algo);
	}

	if (ctx) {
		EVP_MD_CTX_free(ctx);
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
	const char *data = argv[1];

	LOG_INFO("OpenSSL version: %s", OpenSSL_version(OPENSSL_VERSION));
	LOG_INFO("mugglec version: %s", mugglec_version());
	LOG_INFO("mugglec compile time: %s", mugglec_compile_time());

	LOG_INFO("input message: %s", data);

	digest(data, "MD5");
	digest(data, "SHA256");
	digest(data, "SHA512");

	return 0;
}
