#include "unity.h"
#include "foo/foo.h"

void setUp()
{}

void tearDown()
{}

void test_foo_math_add()
{
	int result = foo_math_add(5, 6);
	TEST_ASSERT_EQUAL_INT(result, 5 + 6);
}

void test_foo_math_sub()
{
	int result = foo_math_sub(5, 6);
	TEST_ASSERT_EQUAL_INT(result, 5 - 6);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_foo_math_add);
	RUN_TEST(test_foo_math_sub);

	return UNITY_END();
}
