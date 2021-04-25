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

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, value):
        self._name = value
