import logging
from xml.dom.minidom import Element
from base.singleton import singleton
from base.log_handle import LogHandle
from base.xml_config import XmlConfig


def init_log():
    """
    initialize log
    """
    handler = logging.StreamHandler()
    handler.setLevel(logging.DEBUG)
    handler.setFormatter(logging.Formatter(
        "%(asctime)s|%(name)s|%(levelname)s|%(filename)s:%(lineno)s - "
        "%(message)s"))

    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    logger.addHandler(handler)


@singleton
class HelloXmlConfig(XmlConfig):
    def __init__(self):
        """
        init
        """
        super().__init__()
        self.items = {}

    def load(self, filepath):
        """
        load config path
        """
        dom = self.dom(filepath=filepath)
        if dom is None:
            logging.error("failed get dom")
            return False
        root: Element = dom.documentElement

        node_item_list = root.getElementsByTagName("List")
        if len(node_item_list) != 1:
            logging.warning("failed find node: 'List'")
        else:
            self._init_item_list(node_item_list[0])

        return True

    def _init_item_list(self, node_item_list):
        """
        init item list
        :param node_item_list: List node
        """
        node_items = node_item_list.getElementsByTagName("Item")
        for child in node_items:
            k = self.get_node_attr(child, "k", None)
            v = self.get_node_attr(child, "v", "(null)")
            if k is None:
                logging.error("k is null")
                continue
            self.items[k] = v


if __name__ == "__main__":
    LogHandle.init_log("logs/example_xml.log", console_level=logging.DEBUG)

    cfg = HelloXmlConfig()
    cfg.load("./etc/hello.xml")

    for k, v in cfg.items.items():
        logging.info("{}: {}".format(k, v))
