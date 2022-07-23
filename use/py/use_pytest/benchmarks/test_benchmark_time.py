import time
import pytest


def test_time_time(benchmark):
    benchmark(time.time)


if __name__ == "__main__":
    pytest.main()
