import build.pyfoo as pyfoo


def get_cc_properties(cls):
    properties = {}
    for attr in dir(cls):
        if not callable(getattr(cls, attr)) and \
                not attr.startswith("__") and \
                not attr.startswith("this"):
            properties[attr] = getattr(cls, attr)
    return properties


class MyFooSpi(pyfoo.FooSpi):
    def __init__(self):
        super().__init__()

    def syncCallback(self, x):
        print("sync callback: {}".format(x))

    def asyncCallback(self, x):
        print("async callback: {}".format(x))

    def syncCallbackVar(self, bar):
        print("sync callback var: {}".format(get_cc_properties(bar)))

    def syncCallbackRef(self, bar):
        print("sync callback ref: {}".format(get_cc_properties(bar)))

    def syncCallbackPtr(self, bar):
        print("sync callback ptr: {}".format(get_cc_properties(bar)))

    def onReadFromFile(self, filepath, msg):
        str_filepath = filepath.decode("utf-8")
        if "gbk" in str_filepath:
            content = msg.content.decode("gbk")
        else:
            content = msg.content.decode("utf-8")
        print("read from {}, bytes={}, content={}".format(
            str_filepath, msg.content, content))


if __name__ == "__main__":
    b = b'\xe4\xbd\xa0\xe5\xa5\xbd'
    s = b.decode("utf-8")
    print("bytes={}, content={}".format(b, s))

    api = pyfoo.FooApi()
    spi = MyFooSpi()

    api.setSpi(spi)
    api.syncCall(5)
    api.asyncCall(6)

    bar = pyfoo.Bar()
    bar.i = 5
    bar.f = 10.0
    bar.d = 15.0
    bar.s = "hello world".encode("utf-8")

    api.syncCallVar(bar)
    api.syncCallRef(bar)
    api.syncCallPtr(bar)

    api.readFile("res/utf8.txt".encode("utf-8"))
    api.readFile("res/gbk.txt".encode("utf-8"))
