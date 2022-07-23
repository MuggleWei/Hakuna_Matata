"""
pytest使用setup/teardown function
"""

import pytest


def setup_function():
    print("invoke setup_function()")


def teardown_function():
    print("invoke teardown_function()")


def test_01():
    print("invoke test_01")


def test_02():
    print("invoke test_02")


if __name__ == "__main__":
    pytest.main()
