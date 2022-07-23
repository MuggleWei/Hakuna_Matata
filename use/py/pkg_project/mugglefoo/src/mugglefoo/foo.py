import os

class Foo(object):

    """Foo class"""

    def __init__(self, name="null"):
        """Foo Class init """
        self._name = name

    def sayHello(self, name):
        """say hello to someone.

        :param name: person's name
        "returns: hello message

        """
        return "hello {}, my name is {}".format(name, self._name)

    def get_res_content(self):
        """get content in res/res.txt
        :returns: content in res/res.txt
        """
        filepath = os.path.abspath(__file__)
        folder = os.path.dirname(filepath)
        respath = os.path.join(folder, "res", "res.txt")
        with open(respath, "r") as f:
            return f.read()

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, value):
        self._name = value
