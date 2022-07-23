import pytest


@pytest.fixture(scope="function")
def foo():
    print("initialize foo, scope=function")
    return "foo value"


@pytest.fixture(scope="module")
def bar():
    print("initialize bar, scope=module")
    return "bar value"


def test_with_foo(foo):
    print("test_with_foo: {}".format(foo))


def test_with_bar(bar):
    print("test_with_bar: {}".format(bar))


def test_with_foo_bar1(foo, bar):
    print("test_with_foo_bar1: {}, {}".format(foo, bar))


def test_with_foo_bar2(foo, bar):
    print("test_with_foo_bar2: {}, {}".format(foo, bar))


if __name__ == "__main__":
    pytest.main()
