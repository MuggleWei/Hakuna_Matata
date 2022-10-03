#include "muggle/c/muggle_c.h"

int main()
{
	muggle_log_simple_init(LOG_LEVEL_DEBUG, -1);

	LOG_INFO("hello world");

	return 0;
}
