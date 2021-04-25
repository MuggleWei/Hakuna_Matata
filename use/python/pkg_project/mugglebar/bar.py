from mugglefoo.foo import Foo
from mugglefoo.utils.spam import Spam

if __name__ == "__main__":
    foo = Foo("foo")
    msg = foo.sayHello("bar")
    print(msg)

    spam = Spam()
    ip = spam.get_ip()
    print("ip: {}".format(ip))
