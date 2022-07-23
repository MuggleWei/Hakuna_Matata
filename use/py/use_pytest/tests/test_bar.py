from barproj.bar import Bar

import pytest


@pytest.fixture(scope="function")
def bar():
    return Bar()


def test_bar_fun(bar: Bar):
    assert bar.fun(1) == -1
    assert bar.fun(2) == -2
    # try uncomment function blow and observe cov report
    # assert bar.fun(3) == 3


if __name__ == "__main__":
    pytest.main()
