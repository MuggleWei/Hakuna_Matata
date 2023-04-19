#include "muggle/c/muggle_c.h"
#include "openssl/des.h"

int main(int argc, char *argv[])
{
	muggle_log_simple_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG);

	DES_cblock key;
	DES_random_key(&key);
	key[0] = 0x54;
	key[1] = 0x5e;
	key[2] = 0x3d;
	key[3] = 0x2c;
	key[4] = 0x0e;
	key[5] = 0x2c;
	key[6] = 0x79;
	key[7] = 0xba;

	DES_key_schedule schedule;
	DES_set_key_checked(&key, &schedule);

	const_DES_cblock input = "hello";
	DES_cblock output, plaintext;

	LOG_INFO("input");
	muggle_output_hex(input, sizeof(input), 8);

	LOG_INFO("key");
	muggle_output_hex(key, sizeof(key), 8);

	LOG_INFO("sub keys");
	for (int i = 0; i < 16; i++)
	{
		muggle_output_hex(schedule.ks[i].cblock, 8, 8);
	}

	DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT);
	LOG_INFO("output");
	muggle_output_hex(output, sizeof(output), 8);

	DES_ecb_encrypt(&output, &plaintext, &schedule, DES_DECRYPT);
	LOG_INFO("plaintext");
	muggle_output_hex(plaintext, sizeof(plaintext), 8);

	return 0;
}
