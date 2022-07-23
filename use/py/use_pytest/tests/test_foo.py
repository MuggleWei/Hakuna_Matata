from fooproj.foo import Foo

import pytest


@pytest.fixture(scope="function")
def foo():
    return Foo()


def test_foo_add(foo):
    assert foo.add(5, 6) == 11


def test_foo_sub(foo):
    assert foo.sub(5, 6) == -1


if __name__ == "__main__":
    pytest.main()
