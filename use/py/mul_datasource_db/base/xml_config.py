import os
import xml.dom.minidom


class XmlConfig(object):
    def dom(self, filepath):
        """
        打开文件并获取dom
        :param filepath: 文件路径
        :return:
        """
        if not os.path.exists(filepath):
            return None
        return xml.dom.minidom.parse(file=filepath)

    def get_node_attr(self, node, attr, default_val):
        """
        获取xml节点的属性值
        :param node: xml node
        :param attr: 属性名称
        :param default_val: 默认属性值
        :return:
        """
        if node.hasAttribute(attr):
            return node.getAttribute(attr)
        else:
            return default_val
