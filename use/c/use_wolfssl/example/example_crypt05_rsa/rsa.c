#include "muggle/c/log/log.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/rsa.h"
#include "wolfssl/wolfcrypt/asn.h"
#include "wolfssl/wolfcrypt/asn_public.h"
#include "wolfssl/ssl.h"

#define LARGE_TMP_SZ 4096

byte *load_file(const char *filepath, byte **p_buf, size_t *p_bufsize)
{
	FILE *fp = fopen(filepath, "rb");
	if (fp == NULL) {
		LOG_ERROR("failed open file: %s", filepath);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*p_bufsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*p_buf = (byte *)malloc(*p_bufsize);
	if (fread(*p_buf, 1, *p_bufsize, fp) != *p_bufsize) {
		LOG_ERROR("failed read file: %s", filepath);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	return *p_buf;
}

bool rsa_public_encrypt(const char *filepath, const char *input)
{
	byte *buf = NULL;
	size_t buf_len = 0;

	if (load_file(filepath, &buf, &buf_len) == NULL) {
		LOG_ERROR("failed load file: %s", filepath);
		return false;
	}

	byte der[LARGE_TMP_SZ];
	int der_size = wc_PubKeyPemToDer(buf, buf_len, der, sizeof(der));
	if (der_size < 0) {
		LOG_ERROR("failed convert PEM to DER");
		free(buf);
		return false;
	}

	free(buf);

	LOG_INFO("total DER size: %d", der_size);
	DecodedCert cert;

	RsaKey key;
	memset(&key, 0, sizeof(key));
	int ret = wc_InitRsaKey(&key, NULL);
	if (ret != 0) {
		LOG_ERROR("wc_InitRsaKey failed: %s", wc_GetErrorString(ret));
		return false;
	}

	// TODO: Can't load public key
	word32 idx = 0;
	ret = wc_RsaPublicKeyDecode(der, &idx, &key, der_size);
	if (ret) {
		LOG_ERROR("wc_RsaPublicKeyDecode failed: %s", wc_GetErrorString(ret));
		wc_FreeRsaKey(&key);
		return false;
	}

	wc_FreeRsaKey(&key);

	return true;
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
			"  %s enc ./keys/bob.public hello\n"
			"  <=> [OpenSSL 3]: echo hello | openssl pkeyutl -encrypt -pubin -inkey ./keys/bob.public | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
			"  <=> [OpenSSL 1]: echo hello | openssl rsautl -encrypt -pubin -inkey ./keys/bob.public | hexdump -e '256/1 \"%%02x\" \"\\n\"'\n"
			"\n"
			"  %s dec ./keys/bob.private ${hex format ciphertext}\n"
			"  <=> echo -n ${hex format ciphertext} > ciphertext.txt\n"
			"      xxd -r -p ciphertext.txt ciphertext.bin\n"
			"      openssl pkeyutl -decrypt -in ciphertext.bin -inkey ./keys/bob.private\n",
			argv[0], argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	wolfCrypt_Init();

	int ret = 0;

	RNG rng;
	ret = wc_InitRng(&rng);
	if (ret != 0) {
		LOG_ERROR("wc_InitRng failed: %s", wc_GetErrorString(ret));
		return ret;
	}

	const char *action = argv[1];
	const char *keyfile = argv[2];
	const char *input = argv[3];

	if (strncmp(action, "enc", 3) == 0) {
		rsa_public_encrypt(keyfile, input);
	} else if (strncmp(action, "dec", 3) == 0) {
		// TODO:
	} else {
		LOG_ERROR("Invalid action");
		exit(EXIT_FAILURE);
	}

	wc_FreeRng(&rng);

	wolfCrypt_Cleanup();

	return 0;
}
