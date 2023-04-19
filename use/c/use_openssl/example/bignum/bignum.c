#include "muggle/c/muggle_c.h"
#include "openssl/bn.h"
#include "openssl/err.h"

void output_error()
{
	char buf[512];
	unsigned int err_code = ERR_get_error();
	while (err_code)
	{
		ERR_error_string_n(err_code, buf, sizeof(buf));
		LOG_ERROR("%s", buf);
		err_code = ERR_get_error();
	}
}

const char* bignum_add(
	const char *s1, const char *s2, char *ret, size_t bufsize)
{
	unsigned long err_code = 0;
	BIGNUM *n1 = NULL, *n2 = NULL, *n = NULL;
	const char *p = NULL;

	n1 = BN_new();
	n2 = BN_new();
	n = BN_new();
	if (n1 == NULL || n2 == NULL || n == NULL)
	{
		LOG_ERROR("failed BN_new");
		goto bignum_add_end;
	}

	if (!BN_dec2bn(&n1, s1))
	{
		output_error();
		goto bignum_add_end;
	}

	if (!BN_dec2bn(&n2, s2))
	{
		output_error();
		goto bignum_add_end;
	}

	if (!BN_add(n, n1, n2))
	{
		output_error();
		goto bignum_add_end;
	}

	const char *result = BN_bn2dec(n);
	size_t len = strlen(result);
	if (len + 1 >= bufsize)
	{
		LOG_ERROR("result bufsize is not enough");
		goto bignum_add_end;
	}

	strncpy(ret, result, bufsize - 1);
	ret[len] = '\0';

	p = ret;

bignum_add_end:
	if (n)
	{
		BN_free(n);
	}

	if (n2)
	{
		BN_free(n2);
	}

	if (n1)
	{
		BN_free(n1);
	}

	return p;
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	char buf[512];
	if (bignum_add("999999999999999999999999999999", "1", buf, sizeof(buf)))
	{
		LOG_INFO("result=%s", buf);
	}

	return 0;
}
