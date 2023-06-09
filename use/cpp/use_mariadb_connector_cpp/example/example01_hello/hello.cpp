#include "muggle/c/muggle_c.h"
#include "openssl/opensslv.h"
#include "zlib.h"
#include "mysql.h"
#include "mariadb/conncpp.hpp"

int main()
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	LOG_INFO("mugglec version: %s", mugglec_version());
	LOG_INFO("OpenSSL version: %s", OPENSSL_VERSION_TEXT);
	LOG_INFO("Zlib version: %s", zlibVersion());
	LOG_INFO("mariadb-connector-c version: %s", mysql_get_client_info());
	LOG_INFO("mariadb-connector-c thread-safe: %s",
			 mysql_thread_safe() ? "true" : "false");

	sql::Driver *driver = sql::mariadb::get_driver_instance();
	LOG_INFO("mariadb-connector-cpp version: %u.%u", driver->getMajorVersion(),
			 driver->getMinorVersion());

	return 0;
}
