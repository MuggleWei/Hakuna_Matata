#include "muggle/c/muggle_c.h"
#include "openssl/des.h"

int main(int argc, char *argv[])
{
	muggle_log_simple_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG);

	char *keystring = "hello world";
	DES_cblock key;
	DES_string_to_key(keystring, &key);

	DES_key_schedule schedule;
	if (DES_set_key_checked(&key, &schedule) != 0)
	{
		LOG_ERROR("convert to key_schedule failed");
		exit(EXIT_FAILURE);
	}

	unsigned char input[] = "this is plaintext";
	size_t len = ((sizeof(input) + 7) / 8) * 8;
	unsigned char *output = (unsigned char*)malloc(len);
	unsigned char *plaintext = (unsigned char*)malloc(len + 1);

	LOG_INFO("input");
	muggle_output_hex(input, len, 8);

	DES_cblock iv;
	memset(&iv, 0, sizeof(iv));

	DES_ncbc_encrypt(input, output, sizeof(input), &schedule, &iv, DES_ENCRYPT);
	LOG_INFO("output");
	muggle_output_hex(output, len, 8);

	memset(&iv, 0, sizeof(iv));
	DES_ncbc_encrypt(output, plaintext, len, &schedule, &iv, DES_DECRYPT);
	plaintext[len] = '\0';

	LOG_INFO("plaintext: %s", plaintext);
	muggle_output_hex(plaintext, len, 8);

	free(plaintext);
	free(output);

	return 0;
}
