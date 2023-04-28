#include "unity.h"
#include <stdlib.h>
#include "foo/foo.h"

void setUp()
{}

void tearDown()
{}

void test_foo_str_join()
{
	char *s = foo_str_join("hello ", "world");
	TEST_ASSERT_EQUAL_STRING("hello world", s);
	free(s);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_foo_str_join);

	return UNITY_END();
}
