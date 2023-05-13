#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "muggle/c/muggle_c.h"
#include "psa/crypto.h"

bool hash_msg(const char *input, size_t input_len)
{
	psa_algorithm_t alg = PSA_ALG_SHA_256;
	psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;

	// compute hash of message
	psa_status_t status = psa_hash_setup(&operation, alg);
	if (status != PSA_SUCCESS) {
		LOG_ERROR("Failed in psa_hash_setup");
		return false;
	}

	status = psa_hash_update(&operation, (const uint8_t *)input, input_len);
	if (status != PSA_SUCCESS) {
		LOG_ERROR("Failed in psa_hash_update");
		return false;
	}

	unsigned char actual_hash[PSA_HASH_MAX_SIZE];
	size_t actual_hash_len = 0;
	status = psa_hash_finish(&operation, actual_hash, sizeof(actual_hash),
							 &actual_hash_len);
	if (status != PSA_SUCCESS) {
		LOG_ERROR("Failed in psa_hash_finish");
		return false;
	}

	LOG_INFO("Input message: %s", input);
	char hex[PSA_HASH_MAX_SIZE * 2 + 1];
	muggle_hex_from_bytes(actual_hash, hex, actual_hash_len);
	LOG_INFO("Output hash: %s", hex);

	// clean up hash operation context
	psa_hash_abort(&operation);

	return true;
}

int main(int argc, char *argv[])
{
	if (muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL) != 0) {
		fprintf(stderr, "Failed initialize log");
		exit(EXIT_FAILURE);
	}

	if (argc != 2) {
		LOG_ERROR("Usage: %s <message>", argv[0]);
		exit(EXIT_FAILURE);
	}

	// initialize PSA Crypto
	psa_status_t status = psa_crypto_init();
	if (status != PSA_SUCCESS) {
		LOG_ERROR("Failed to initialize PSA Crypto");
		return false;
	}

	const char *input = (char *)argv[1];
	bool ret = hash_msg(input, strlen(input));

	// Free PSA Crypto
	mbedtls_psa_crypto_free();

	return ret ? 0 : 1;
}
