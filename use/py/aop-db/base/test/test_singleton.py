import os
import unittest

from base.config_utils import ConfigUtils


class TestSingleton(unittest.TestCase):
    def test_config_utils_singleton(self):
        module_path = os.path.dirname(__file__)
        cfg_path = os.path.join(module_path, '../../config')
        cfg_utils = ConfigUtils(cfg_path=cfg_path)
        cfg_utils2 = ConfigUtils(cfg_path='')
        self.assertEquals(cfg_utils, cfg_utils2)


if __name__ == '__main__':
    unittest.main()
