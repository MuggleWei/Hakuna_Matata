"""
pytest使用类的setup/teardown
"""

import pytest


class TestClass(object):
    def setup_class(self):
        print("invoke setup_class()")

    def teardown_class(self):
        print("invoke teardown_class()")

    def setup_method(self):
        print("invoke setup_method()")

    def teardown_method(self):
        print("invoke teardown_method()")

    def test_01(self):
        print("invoke test_01")

    def test_02(self):
        print("invoke test_02")


if __name__ == "__main__":
    pytest.main()
