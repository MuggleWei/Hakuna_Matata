#include "muggle/cpp/muggle_cpp.h"

int main()
{
	muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	LOG_INFO("hello world");

	return 0;
}
