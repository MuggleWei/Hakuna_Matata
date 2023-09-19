#include "muggle/c/muggle_c.h"

int main()
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	LOG_INFO("hello world");

	return 0;
}
