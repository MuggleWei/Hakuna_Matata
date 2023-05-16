#include <muggle/c/log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/build_info.h"
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/error.h"
#include "muggle/c/muggle_c.h"

#if defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_ENTROPY_C)
	#include "mbedtls/entropy.h"
	#include "mbedtls/ctr_drbg.h"
#endif

#if !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_ENTROPY_C)

int main()
{
	fprintf(stderr, "MBEDTLS_CTR_DRBG_C or MBEDTLS_ENTROPY_C not defined\n");
	return 0;
}

#else

bool mbedtls_thread_init(mbedtls_entropy_context *p_entropy,
						 mbedtls_ctr_drbg_context *p_ctr_drbg,
						 char *personalization_str)
{
	// setup the entropy source
	mbedtls_entropy_init(p_entropy);

	// the random generator
	mbedtls_ctr_drbg_init(p_ctr_drbg);
	int ret = mbedtls_ctr_drbg_seed(p_ctr_drbg, mbedtls_entropy_func, p_entropy,
									(const unsigned char *)personalization_str,
									strlen(personalization_str));
	if (ret != 0) {
		LOG_ERROR("failed in mbedtls_ctr_drbg_seed");
		goto cleanup;
	}

	// enabling prediction resistance
	mbedtls_ctr_drbg_set_prediction_resistance(p_ctr_drbg,
											   MBEDTLS_CTR_DRBG_PR_ON);

	return true;

cleanup:
	mbedtls_ctr_drbg_free(p_ctr_drbg);
	mbedtls_entropy_free(p_entropy);

	return false;
}

void mbedtls_thread_cleanup(mbedtls_entropy_context *p_entropy,
							mbedtls_ctr_drbg_context *p_ctr_drbg)
{
	mbedtls_ctr_drbg_free(p_ctr_drbg);
	mbedtls_entropy_free(p_entropy);
}

int rsa_encrypt(mbedtls_ctr_drbg_context *p_ctr_drbg, const char *keyfile,
				const char *plaintext)
{
	int ret = 0;

	mbedtls_pk_context pk;
	mbedtls_pk_init(&pk);

	// read the RSA public key
	ret = mbedtls_pk_parse_public_keyfile(&pk, keyfile);
	if (ret != 0) {
		char errmsg[200];
		mbedtls_strerror(ret, errmsg, sizeof(errmsg));
		LOG_ERROR("Failed mbedtls_pk_parse_public_keyfile, errmsg: %s", errmsg);
		goto exit;
	}

	// calculate the RSA encryption of the data
	unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
	size_t outlen = 0;
	ret = mbedtls_pk_encrypt(&pk, (const unsigned char *)plaintext,
							 strlen(plaintext), buf, &outlen, sizeof(buf),
							 mbedtls_ctr_drbg_random, p_ctr_drbg);
	if (ret != 0) {
		char errmsg[200];
		mbedtls_strerror(ret, errmsg, sizeof(errmsg));
		LOG_ERROR("Failed mbedtls_pk_encrypt, errmsg: %s", errmsg);
		goto exit;
	}

	// output hex string
	char hexbuf[MBEDTLS_MPI_MAX_SIZE * 2 + 1];
	muggle_hex_from_bytes((const uint8_t *)buf, hexbuf, (uint32_t)outlen);
	LOG_INFO("output bytes len: %u\n%s", (unsigned int)outlen, hexbuf);

exit:
	mbedtls_pk_free(&pk);

	return ret;
}

int rsa_decrypt(mbedtls_ctr_drbg_context *p_ctr_drbg, const char *keyfile,
				const char *ciphertext)
{
	int ret = 0;

	mbedtls_pk_context pk;
	mbedtls_pk_init(&pk);

	// read the RSA private key
	ret = mbedtls_pk_parse_keyfile(&pk, keyfile, "", mbedtls_ctr_drbg_random,
								   p_ctr_drbg);
	if (ret != 0) {
		char errmsg[200];
		mbedtls_strerror(ret, errmsg, sizeof(errmsg));
		LOG_ERROR("Failed mbedtls_pk_parse_keyfile, errmsg: %s", errmsg);
		goto exit;
	}

	// convert hex ciphertext string to bytes
	unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
	size_t ciphtertext_len = strlen(ciphertext);
	muggle_hex_to_bytes(ciphertext, buf, strlen(ciphertext));
	size_t bytes_len = ciphtertext_len / 2;

	// extract the RSA encrypted value from the hex format text
	unsigned char plaintext[MBEDTLS_MPI_MAX_SIZE];
	size_t outlen = 0;
	ret = mbedtls_pk_decrypt(&pk, buf, bytes_len, plaintext, &outlen,
							 sizeof(plaintext), mbedtls_ctr_drbg_random,
							 p_ctr_drbg);
	if (ret != 0) {
		char errmsg[200];
		mbedtls_strerror(ret, errmsg, sizeof(errmsg));
		LOG_ERROR("Failed mbedtls_pk_decrypt, errmsg: %s", errmsg);
		goto exit;
	}

	// output plaintext
	LOG_INFO("output bytes len: %u\n%s", (unsigned int)outlen, plaintext);

exit:
	mbedtls_pk_free(&pk);
	return ret;
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc != 4) {
		LOG_INFO(
			"Usage: %s <enc|dec> <key> <plaintext|ciphertext>\n"
			"  Example of RSA.\n"
			"  Use Public Key to encrypt and private key to decrypt\n"
			"  Before run example, use gen_keys.sh to generate key pair\n"
			"e.g.\n"
			"  %s enc ./etc/rsa.public hello\n"
			"  <=> [OpenSSL 3]: echo hello | openssl pkeyutl -encrypt -pubin -inkey ./etc/rsa.public | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
			"  <=> [OpenSSL 1]: echo hello | openssl rsautl -encrypt -pubin -inkey ./etc/rsa.public | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
			"\n"
			"  %s dec ./etc/rsa.private ${hex format ciphertext}\n"
			"  <=> echo -n ${hex format ciphertext} > ciphertext.txt\n"
			"      xxd -r -p ciphertext.txt ciphertext.bin\n"
			"      openssl pkeyutl -decrypt -in ciphertext.bin -inkey ./etc/rsa.private\n",
			argv[0], argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	char *personalization = "mbedtls examples";

	if (!mbedtls_thread_init(&entropy, &ctr_drbg, personalization)) {
		LOG_ERROR("Failed mbedtls thread init");
		exit(EXIT_FAILURE);
	}

	const char *action = argv[1];
	const char *keyfile = argv[2];
	const char *input = argv[3];
	int ret = 0;
	if (strncmp(action, "enc", 3) == 0) {
		ret = rsa_encrypt(&ctr_drbg, keyfile, input);
	} else if (strncmp(action, "dec", 3) == 0) {
		ret = rsa_decrypt(&ctr_drbg, keyfile, input);
	} else {
		LOG_ERROR("Invalid action");
		exit(EXIT_FAILURE);
	}

	mbedtls_thread_cleanup(&entropy, &ctr_drbg);

	return ret;
}

#endif
