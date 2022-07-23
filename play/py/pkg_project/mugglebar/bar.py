from mugglefoo.foo import Foo
from mugglefoo.utils.spam import Spam

if __name__ == "__main__":
    foo = Foo("foo")
    msg = foo.sayHello("bar")
    print(msg)

    res_content = foo.get_res_content()
    print("res content: {}".format(res_content))

    spam = Spam()
    ip = spam.get_ip()
    print("ip: {}".format(ip))
