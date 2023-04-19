#include "muggle/c/muggle_c.h"
#include "openssl/ssl.h"

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	LOG_INFO("mugglec version: mugglec %s %s",
		mugglec_version(), mugglec_compile_time());
	LOG_INFO("openssl version: %s", OpenSSL_version(OPENSSL_VERSION));

	return 0;
}
