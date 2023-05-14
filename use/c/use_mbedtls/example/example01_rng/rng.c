#include <muggle/c/log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/build_info.h"
#include "mbedtls/platform.h"
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

int main()
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	int ret = 0;

	// setup the entropy source
	mbedtls_entropy_context entropy;
	mbedtls_entropy_init(&entropy);

	// the random generator
	mbedtls_ctr_drbg_context ctr_drbg;
	char *personalization = "mbedtls examples"; // app specific string
	mbedtls_ctr_drbg_init(&ctr_drbg);
	ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
								(const unsigned char *)personalization,
								strlen(personalization));
	if (ret != 0) {
		LOG_ERROR("failed in mbedtls_ctr_drbg_seed");
		goto cleanup;
	}

	// enabling prediction resistance
	mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg,
											   MBEDTLS_CTR_DRBG_PR_ON);

	// generate random data
	unsigned char buf[32];
	for (int i = 0; i < 8; i++) {
		ret = mbedtls_ctr_drbg_random(&ctr_drbg, buf, sizeof(buf));
		if (ret != 0) {
			LOG_ERROR("failed in mbedtls_ctr_drbg_random");
			goto cleanup;
		}

		LOG_INFO("Generating %ld bytes of data", (long)sizeof(buf));
		muggle_output_hex(buf, sizeof(buf), 8);
	}

cleanup:
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

	return ret;
}

#endif
