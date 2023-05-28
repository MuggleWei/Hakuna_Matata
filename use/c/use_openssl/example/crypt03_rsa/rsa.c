#include <string.h>
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/evp.h"
#include "openssl/pem.h"
#include "muggle/c/encoding/hex.h"
#include "muggle/c/log/log.h"
#include "muggle/c/os/path.h"
#include "muggle/c/version/version.h"

EVP_PKEY_CTX *load_pub_key(const char *key_filepath)
{
	EVP_PKEY_CTX *ctx = NULL;

	BIO *bp = NULL;
	EVP_PKEY *key = NULL;

	int ret = 0;

	bp = BIO_new(BIO_s_file());
	if (bp == NULL) {
		LOG_ERROR("BIO_new(BIO_s_file()) failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto load_pub_key_exit;
	}

	ret = BIO_read_filename(bp, key_filepath);
	if (ret <= 0) {
		LOG_ERROR("BIO_read_filename failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto load_pub_key_exit;
	}

	// set key context
	key = EVP_PKEY_new();
	if (PEM_read_bio_PUBKEY_ex(bp, &key, NULL, NULL, NULL, NULL) == NULL) {
		LOG_ERROR("PEM_read_bio_PUBKEY_ex failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto load_pub_key_exit;
	}

	ctx = EVP_PKEY_CTX_new(key, NULL);
	if (ctx == NULL) {
		LOG_ERROR("EVP_PKEY_CTX_new failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto load_pub_key_exit;
	}

	ret = EVP_PKEY_encrypt_init(ctx);
	if (ret <= 0) {
		LOG_ERROR("EVP_PKEY_encrypt_init failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto load_pub_key_exit;
	}

	ret = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
	if (ret <= 0) {
		LOG_ERROR("EVP_PKEY_CTX_set_rsa_padding failed: %s",
				  ERR_reason_error_string(ERR_get_error()));

		EVP_PKEY_CTX_free(ctx);
		ctx = NULL;

		goto load_pub_key_exit;
	}

load_pub_key_exit:
	if (key) {
		EVP_PKEY_free(key);
	}

	if (bp) {
		BIO_free(bp);
	}

	return ctx;
}

char *pub_encrypt(const char *pubkey, const char *plaintext)
{
	char *ciphertext_hex = NULL;

	int ret = 0;

	EVP_PKEY_CTX *enc_ctx = NULL;
	unsigned char *ciphertext = NULL;

	enc_ctx = load_pub_key(pubkey);
	if (enc_ctx == NULL) {
		LOG_ERROR("failed load alice public key ctx");
		goto pub_encrypt_exit;
	}

	// encrypt
	size_t outlen;
	ret = EVP_PKEY_encrypt(enc_ctx, NULL, &outlen,
						   (const unsigned char *)plaintext, strlen(plaintext));
	if (ret <= 0) {
		LOG_ERROR("failed get ciphertext length");
		goto pub_encrypt_exit;
	}

	ciphertext = (unsigned char *)OPENSSL_malloc(outlen);
	ret = EVP_PKEY_encrypt(enc_ctx, ciphertext, &outlen,
						   (const unsigned char *)plaintext, strlen(plaintext));
	if (ret <= 0) {
		LOG_ERROR("failed get ciphertext length");
		goto pub_encrypt_exit;
	}

	// convert to hex format
	ciphertext_hex = (char *)malloc(outlen * 2 + 1);
	muggle_hex_from_bytes(ciphertext, ciphertext_hex, outlen);
	ciphertext_hex[outlen * 2] = '\0';

pub_encrypt_exit:
	if (ciphertext) {
		OPENSSL_free(ciphertext);
	}

	if (enc_ctx) {
		EVP_PKEY_CTX_free(enc_ctx);
	}

	return ciphertext_hex;
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
	const char *alice_pri_name = "alice.private";
	const char *bob_pub_name = "bob.public";
	const char *bob_pri_name = "bob.private";
	char alice_pub[MUGGLE_MAX_PATH];
	char alice_pri[MUGGLE_MAX_PATH];
	char bob_pub[MUGGLE_MAX_PATH];
	char bob_pri[MUGGLE_MAX_PATH];
	muggle_path_join(key_dir, alice_pub_name, alice_pub, sizeof(alice_pub));
	muggle_path_join(key_dir, alice_pri_name, alice_pri, sizeof(alice_pri));
	muggle_path_join(key_dir, bob_pub_name, bob_pub, sizeof(bob_pub));
	muggle_path_join(key_dir, bob_pri_name, bob_pri, sizeof(bob_pri));

	char *ciphertext_hex = pub_encrypt(alice_pub, plaintext);

	LOG_INFO(
		"alice public key[%s] encrypt\n"
		"  <=> [OpenSSL 3]: echo -n \"%s\" | openssl pkeyutl -encrypt -pubin -inkey %s | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
		"  <=> [OpenSSL 1]: echo -n \"%s\" | openssl rsautl -encrypt -pubin -inkey %s | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
		"%s",
		alice_pub, plaintext, alice_pub, plaintext, alice_pub, ciphertext_hex);

	// TODO:

	free(ciphertext_hex);

	return 0;
}
