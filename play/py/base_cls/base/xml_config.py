import os
import xml.dom.minidom


class XmlConfig(object):
    """
    XML 配置文件对象
    """

    def dom(self, filepath):
        """
        打开文件并获取 dom
        :param filepath: xml 文件路径
        :return: 成功时返回 dom 对象, 否则返回 None
        """
        if not os.path.exists(filepath):
            return None
        return xml.dom.minidom.parse(file=filepath)

    def get_node_attr(self, node, attr, default_val):
        """
        获取属性值
        :param node: xml 节点
        :param attr: 属性名称
        :param default_val: 默认值
        """
        if node.hasAttribute(attr):
            return node.getAttribute(attr)
        else:
            return default_val
