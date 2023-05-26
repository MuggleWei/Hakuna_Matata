#include "muggle/c/log/log.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/rsa.h"
#include "wolfssl/ssl.h"

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

	int ret = 0;

	RNG rng;
	ret = wc_InitRng(&rng);
	if (ret != 0) {
		LOG_ERROR("failed init wolfssl RNG");
		return ret;
	}

	// TODO:

	return 0;
}
