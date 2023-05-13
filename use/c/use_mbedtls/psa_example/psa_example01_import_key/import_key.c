#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "muggle/c/muggle_c.h"
#include "psa/crypto.h"

bool import_key_aes_128(const uint8_t *key, size_t keylen)
{
	size_t keylen_bits = keylen * 8;
	if (keylen_bits != 128 && keylen_bits != 192 && keylen_bits != 256) {
		LOG_ERROR("Invalid AES key length: %lu bits",
				  (unsigned long)keylen_bits);
		return false;
	}

	// set key attributes
	psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_usage_flags(&attributes, 0);
	psa_set_key_algorithm(&attributes, 0);
	psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&attributes, keylen_bits);

	// import the key
	psa_key_id_t key_id;
	psa_status_t status = psa_import_key(&attributes, key, keylen, &key_id);
	if (status != PSA_SUCCESS) {
		LOG_ERROR("Failed to import key");
		return false;
	}

	LOG_INFO("Success import a AES %lu key", (unsigned long)keylen_bits);

	// free the attributes
	psa_reset_key_attributes(&attributes);

	// destroy the key
	psa_destroy_key(key_id);

	return true;
}

int main()
{
	if (muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL) != 0) {
		fprintf(stderr, "Failed initialize log");
		exit(EXIT_FAILURE);
	}

	// initialize PSA Crypto
	psa_status_t status = psa_crypto_init();
	if (status != PSA_SUCCESS) {
		LOG_ERROR("Failed to initialize PSA Crypto");
		return false;
	}

	const char *aes_key = "0123456789abcdef";
	bool ret = import_key_aes_128((const uint8_t *)aes_key,
								  strlen((const char *)aes_key));

	// Free PSA Crypto
	mbedtls_psa_crypto_free();

	return ret ? 0 : 1;
}
