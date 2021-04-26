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

    def test_get_res(self):
        content = self._foo.get_res_content()
        content = content.strip()
        expect_str = "foo"
        self.assertEqual(content, expect_str)


if __name__ == "__main__":
    unittest.main()
