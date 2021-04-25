import unittest
from src.mugglefoo.foo import Foo


class TestFoo(unittest.TestCase):
    def setUp(self):
        self._foo = Foo("unknown")

    def test_name(self):
        self.assertEqual(self._foo.name, "unknown")

        self._foo.name = "foo"
        self.assertEqual(self._foo.name, "foo")

    def test_say_hello(self):
        msg = self._foo.sayHello("bar")
        expect_str = "hello {}, my name is {}".format(
            "bar", "unknown")
        self.assertEqual(msg, expect_str)


if __name__ == "__main__":
    unittest.main()
