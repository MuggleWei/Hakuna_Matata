#include "muggle/c/muggle_c.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/wolfcrypt/aes.h"
#include "wolfssl/ssl.h"

int random_block(byte *buf, uint32_t bufsize)
{
	int ret = 0;

	RNG rng;
	ret = wc_InitRng(&rng);
	if (ret != 0) {
		LOG_ERROR("wc_InitRng failed: %s", wc_GetErrorString(ret));
		return ret;
	}

	ret = wc_RNG_GenerateBlock(&rng, buf, bufsize);
	if (ret != 0) {
		LOG_ERROR("wc_RNG_GenerateBlock failed: %s", wc_GetErrorString(ret));
		return ret;
	}

	// free rng
	ret = wc_FreeRng(&rng);
	if (ret != 0) {
		LOG_ERROR("wc_FreeRng failed: %s", wc_GetErrorString(ret));
		return ret;
	}

	return 0;
}

void aes_cbc(byte *key, size_t keysize, byte *iv, const char *plaintext)
{
	int ret = 0;

	size_t len = strlen(plaintext);

	// NOTE: padding plaintext is required if in ECB/CBC block cipher mode
	if (len % AES_BLOCK_SIZE != 0) {
		len = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	}

	// encrypt
	Aes enc;
	ret = wc_AesSetKey(&enc, key, keysize, iv, AES_ENCRYPTION);
	if (ret != 0) {
		LOG_ERROR("wc_AesSetKey failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	byte *ciphertext = (byte *)malloc(len);
	ret = wc_AesCbcEncrypt(&enc, ciphertext, (const byte *)plaintext, len);
	if (ret != 0) {
		LOG_ERROR("wc_AesCbcEncrypt failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	// decrypt
	Aes dec;
	ret = wc_AesSetKey(&dec, key, keysize, iv, AES_DECRYPTION);
	if (ret != 0) {
		LOG_ERROR("wc_AesSetKey failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}

	byte *ret_plaintext = (byte *)malloc(len + 1);
	ret = wc_AesCbcDecrypt(&dec, ret_plaintext, (const byte *)ciphertext, len);
	if (ret != 0) {
		LOG_ERROR("wc_AesCbcDecrypt failed: %s", wc_GetErrorString(ret));
		exit(EXIT_FAILURE);
	}
	ret_plaintext[len] = '\0';

	char *ciphertext_hex = (char *)malloc(len * 2 + 1);
	muggle_hex_from_bytes(ciphertext, ciphertext_hex, len);
	ciphertext_hex[len * 2] = '\0';

	LOG_INFO("AES CBC\n"
			 "plaintext: %s\n"
			 "ciphertext: %s\n"
			 "ret plaintext: %s",
			 plaintext, ciphertext_hex, ret_plaintext);

	free(ciphertext_hex);
	free(ret_plaintext);
	free(ciphertext);
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc < 2) {
		LOG_ERROR("Usage: %s <plaintext>", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char *plaintext = argv[1];

	// generate AES key and iv
	byte key[AES_256_KEY_SIZE];
	if (random_block(key, sizeof(key)) != 0) {
		LOG_ERROR("failed generate AES key");
		exit(EXIT_FAILURE);
	}
	LOG_INFO("generate AES key: ");
	muggle_output_hex(key, sizeof(key), sizeof(key));

	byte iv[AES_IV_SIZE];
	if (random_block(iv, sizeof(iv)) != 0) {
		LOG_ERROR("failed generate iv");
		exit(EXIT_FAILURE);
	}
	LOG_INFO("generate AES iv: ");
	muggle_output_hex(iv, sizeof(iv), sizeof(iv));

	// cbc
	aes_cbc(key, sizeof(key), iv, plaintext);

	return 0;
}
