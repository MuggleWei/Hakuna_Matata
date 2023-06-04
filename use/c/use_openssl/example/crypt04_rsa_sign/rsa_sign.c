#include <stdlib.h>
#include "muggle/c/encoding/hex.h"
#include "muggle/c/log/log.h"
#include "muggle/c/os/path.h"
#include "muggle/c/version/version.h"
#include "openssl/crypto.h"
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/evp.h"
#include "openssl/pem.h"

unsigned char *digest(const char *plaintext, const EVP_MD *algo,
					  unsigned int *p_outlen)
{
	EVP_MD_CTX *ctx = NULL;
	unsigned char *outdigest = NULL;

	ctx = EVP_MD_CTX_new();
	if (ctx == NULL) {
		LOG_ERROR("EVP_MD_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	if (!EVP_DigestInit_ex(ctx, algo, NULL)) {
		LOG_ERROR("EVP_DigestInit_ex failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	if (!EVP_DigestUpdate(ctx, plaintext, strlen(plaintext))) {
		LOG_ERROR("EVP_DigestUpdate failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	outdigest = OPENSSL_malloc(EVP_MD_get_size(algo));
	if (outdigest == NULL) {
		LOG_ERROR("OPENSSL_malloc failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto digest_exit;
	}

	if (!EVP_DigestFinal(ctx, outdigest, p_outlen)) {
		LOG_ERROR("EVP_DigestFinal failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		free(outdigest);
		outdigest = NULL;

		goto digest_exit;
	}

digest_exit:
	if (ctx) {
		EVP_MD_CTX_free(ctx);
	}

	return outdigest;
}

EVP_PKEY_CTX *init_sign_ctx(const char *pem_filepath, const char *passphrase,
							const EVP_MD *algo)
{
	EVP_PKEY_CTX *ctx = NULL;

	BIO *bp = NULL;
	EVP_PKEY *key = NULL;

	int ret = 0;

	// add all algorithms
	bp = BIO_new(BIO_s_file());
	if (bp == NULL) {
		LOG_ERROR("BIO_new(BIO_s_file()) failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	// read key file
	ret = BIO_read_filename(bp, pem_filepath);
	if (ret <= 0) {
		LOG_ERROR("BIO_read_filename failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	// set key context
	key = EVP_PKEY_new();
	if (PEM_read_bio_PrivateKey(bp, &key, NULL, (void *)passphrase) == NULL) {
		LOG_ERROR("PEM_read_bio_PrivateKey failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	ctx = EVP_PKEY_CTX_new(key, NULL);
	if (ctx == NULL) {
		LOG_ERROR("EVP_PKEY_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	ret = EVP_PKEY_sign_init(ctx);
	if (ret <= 0) {
		LOG_ERROR("EVP_PKEY_sign_init failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto init_ctx_exit;
	}

	ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
	if (ret <= 0) {
		LOG_ERROR("EVP_PKEY_CTX_set_rsa_padding failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto init_ctx_exit;
	}

	if (EVP_PKEY_CTX_set_signature_md(ctx, algo) <= 0) {
		LOG_ERROR("EVP_PKEY_CTX_set_signature_md failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto init_ctx_exit;
	}

init_ctx_exit:
	if (key) {
		EVP_PKEY_free(key);
	}

	if (bp) {
		BIO_free(bp);
	}

	return ctx;
}

EVP_PKEY_CTX *init_verify_ctx(const char *pem_filepath, const EVP_MD *algo)
{
	EVP_PKEY_CTX *ctx = NULL;

	BIO *bp = NULL;
	EVP_PKEY *key = NULL;

	int ret = 0;

	// add all algorithms
	bp = BIO_new(BIO_s_file());
	if (bp == NULL) {
		LOG_ERROR("BIO_new(BIO_s_file()) failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	// read key file
	ret = BIO_read_filename(bp, pem_filepath);
	if (ret <= 0) {
		LOG_ERROR("BIO_read_filename failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	// set key context
	key = EVP_PKEY_new();
	if (PEM_read_bio_PUBKEY_ex(bp, &key, NULL, NULL, NULL, NULL) == NULL) {
		LOG_ERROR("PEM_read_bio_PUBKEY_ex failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	ctx = EVP_PKEY_CTX_new(key, NULL);
	if (ctx == NULL) {
		LOG_ERROR("EVP_PKEY_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto init_ctx_exit;
	}

	ret = EVP_PKEY_verify_init(ctx);
	if (ret <= 0) {
		LOG_ERROR("EVP_PKEY_encrypt_init failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto init_ctx_exit;
	}

	ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
	if (ret <= 0) {
		LOG_ERROR("EVP_PKEY_CTX_set_rsa_padding failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto init_ctx_exit;
	}

	if (EVP_PKEY_CTX_set_signature_md(ctx, algo) <= 0) {
		LOG_ERROR("EVP_PKEY_CTX_set_signature_md failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto init_ctx_exit;
	}

init_ctx_exit:
	if (key) {
		EVP_PKEY_free(key);
	}

	if (bp) {
		BIO_free(bp);
	}

	return ctx;
}

char *sign(const char *pem_filepath, const char *passphrase,
		   const char *plaintext)
{
	char *sign_hex = NULL;

	int ret = 0;

	unsigned char *outdigest = NULL;
	unsigned int digest_len = 0;

	EVP_PKEY_CTX *sign_ctx = NULL;
	unsigned char *sign_bytes = NULL;

	// digest
	const EVP_MD *algo = EVP_sha256();
	outdigest = digest(plaintext, algo, &digest_len);
	if (outdigest == NULL) {
		LOG_ERROR("failed digest");
		goto sign_exit;
	}

	sign_ctx = init_sign_ctx(pem_filepath, passphrase, algo);
	if (sign_ctx == NULL) {
		LOG_ERROR("failed init sign ctx");
		goto sign_exit;
	}

	// sign
	size_t outlen;
	ret = EVP_PKEY_sign(sign_ctx, NULL, &outlen,
						(const unsigned char *)plaintext, strlen(plaintext));
	if (ret <= 0) {
		LOG_ERROR("failed get sign length: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto sign_exit;
	}

	sign_bytes = (unsigned char *)malloc(outlen + 1);
	sign_bytes[outlen] = '\0';
	ret = EVP_PKEY_sign(sign_ctx, sign_bytes, &outlen, outdigest, digest_len);
	if (ret <= 0) {
		LOG_ERROR("failed sign: %s", ERR_reason_error_string(ERR_get_error()));

		free(sign_bytes);
		sign_bytes = NULL;

		goto sign_exit;
	}

	// convert to hex format
	sign_hex = (char *)malloc(outlen * 2 + 1);
	muggle_hex_from_bytes(sign_bytes, sign_hex, outlen);
	sign_hex[outlen * 2] = '\0';

sign_exit:
	if (sign_bytes) {
		free(sign_bytes);
	}

	if (sign_ctx) {
		EVP_PKEY_CTX_free(sign_ctx);
	}

	if (outdigest) {
		free(outdigest);
	}

	return (char *)sign_hex;
}

bool verify(const char *pem_filepath, const char *plaintext,
			const char *sign_hex)
{
	bool verify_ret = true;

	unsigned char *outdigest = NULL;
	unsigned int digest_len = 0;

	EVP_PKEY_CTX *verify_ctx = NULL;

	unsigned char *sig = NULL;

	// digest
	const EVP_MD *algo = EVP_sha256();
	outdigest = digest(plaintext, algo, &digest_len);
	if (outdigest == NULL) {
		LOG_ERROR("failed digest");
		goto verify_exit;
	}

	verify_ctx = init_verify_ctx(pem_filepath, algo);
	if (verify_ctx == NULL) {
		LOG_ERROR("failed init verify ctx");
		goto verify_exit;
	}

	size_t siglen = strlen(sign_hex) / 2;
	sig = (unsigned char *)malloc(siglen);
	muggle_hex_to_bytes(sign_hex, sig, strlen(sign_hex));

	int ret = EVP_PKEY_verify(verify_ctx, sig, siglen, outdigest, digest_len);
	if (ret == 0) {
		verify_ret = false;
	} else if (ret < 0) {
		LOG_ERROR("failed EVP_PKEY_verify: %s",
				  ERR_reason_error_string(ERR_get_error()));
		verify_ret = false;
	}

verify_exit:
	if (sig) {
		free(sig);
	}

	if (outdigest) {
		free(outdigest);
	}

	if (verify_ctx) {
		EVP_PKEY_CTX_free(verify_ctx);
	}

	return verify_ret;
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc < 3) {
		LOG_ERROR("Usage: %s <key dir> <message>\n"
				  "e.g.\n"
				  "  %s ./keys \"hello world\" ",
				  argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	const char *key_dir = argv[1];
	const char *plaintext = argv[2];

	LOG_INFO("mugglec version: %s", mugglec_version());
	LOG_INFO("mugglec compile time: %s", mugglec_compile_time());
	LOG_INFO("OpenSSL version: %s", OpenSSL_version(OPENSSL_VERSION));

	// prepare keys
	const char *alice_pub_name = "alice.public";
	const char *alice_pri_name = "alice.enc.private";
	const char *alice_pri_passphrase = "hello123";
	const char *bob_pub_name = "bob.public";
	const char *bob_pri_name = "bob.enc.private";
	const char *bob_pri_passphrase = "hello123";
	char alice_pub[MUGGLE_MAX_PATH];
	char alice_pri[MUGGLE_MAX_PATH];
	char bob_pub[MUGGLE_MAX_PATH];
	char bob_pri[MUGGLE_MAX_PATH];
	muggle_path_join(key_dir, alice_pub_name, alice_pub, sizeof(alice_pub));
	muggle_path_join(key_dir, alice_pri_name, alice_pri, sizeof(alice_pri));
	muggle_path_join(key_dir, bob_pub_name, bob_pub, sizeof(bob_pub));
	muggle_path_join(key_dir, bob_pri_name, bob_pri, sizeof(bob_pri));

	// sign
	char *sign_hex = sign(bob_pri, bob_pri_passphrase, plaintext);
	if (sign_hex == NULL) {
		LOG_ERROR("failed sign");
		exit(EXIT_FAILURE);
	}

	LOG_INFO(
		"bob private key[%s] sign\n"
		"  <=> echo -n \"%s\" | openssl dgst -sha256 -sign %s -passin \"pass:%s\" | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
		"output: %s",
		bob_pri, plaintext, bob_pri, bob_pri_passphrase, sign_hex);

	// verify
	LOG_INFO(
		"bob public key[%s] verify\n"
		"  <=> echo -n ${hex format sign} > sign.txt\n"
		"      xxd -r -p sign.txt sign.bin\n"
		"      echo -n \"%s\" | openssl dgst -sha256 -verify %s -signature sign.bin",
		bob_pub, plaintext, bob_pub);
	if (verify(bob_pub, plaintext, sign_hex)) {
		LOG_INFO("Verified Ok");
	} else {
		LOG_ERROR("Verifed failure");
	}

	free(sign_hex);

	return 0;
}
