/******************************************************************************
 *  @file         hello.c
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2022-11-15
 *  @copyright    Copyright 2022 Muggle Wei
 *  @license      MIT License
 *  @brief        
 *****************************************************************************/
#include "muggle/c/muggle_c.h"

struct foo
{
	uint32_t u32;
	char buf[16];
};

int main()
{
	// init log
	muggle_log_complicated_init(LOG_LEVEL_INFO, LOG_LEVEL_INFO, NULL);

	LOG_INFO("hello world");
	LOG_WARNING("hello world");
	LOG_ERROR("hello world");

	struct foo f = {
		.u32 = 5,
		.buf = {"abc"}
	};
	LOG_INFO("foo.u32=%u, foo.buf=%s", f.u32, f.buf);

	return 0;
}
