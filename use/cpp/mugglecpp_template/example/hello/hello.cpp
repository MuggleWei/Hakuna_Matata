#include "muggle/cpp/muggle_cpp.h"

int main()
{
	muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	LOG_DEBUG("hello mugglecpp");
	
	return 0;
}
