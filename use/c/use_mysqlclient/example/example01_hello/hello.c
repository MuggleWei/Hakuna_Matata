#include <stdio.h>
#include "mysql.h"
#include "muggle/c/muggle_c.h"

int main()
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	LOG_INFO("MYSQL client version: %s", mysql_get_client_info());
	LOG_INFO("MYSQL client thread-safe: %s",
			 mysql_thread_safe() ? "true" : "false");

	return 0;
}
