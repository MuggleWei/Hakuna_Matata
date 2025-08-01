import logging
import shutil
import subprocess
import sys


def check_commands_exists():
    """
    检测使用到的外部命令是否存在
    """
    required_cmd = [
        "ifconfig",
        "ethtool",
        "lspci",
        "dmidecode",
    ]
    for cmd in required_cmd:
        if not shutil.which(cmd):
            print("failed find command: {}".format(cmd))
            return False
    return True


def parse_ifconfig_result(lines):
    """
    解析 ifconfig 的输出
    :param lines: 所有的输出行
    """
    iface = None
    info = {}
    iface_dict = {}
    for line in lines:
        cur_line = line.strip()
        if len(cur_line) == 0:
            if iface is not None:
                iface_dict[iface] = info
                iface = None
                info = {}
            continue
        if iface is None:
            vec = cur_line.split(":")
            if len(vec) == 2:
                iface = vec[0].strip()
        else:
            kv_vec = cur_line.split(" ")
            key = None
            for s in kv_vec:
                v = s.strip()
                if len(v) == 0:
                    continue
                if key is None:
                    key = v
                else:
                    info[key] = v
                    key = None
    if iface is not None:
        iface_dict[iface] = info
        iface = None
        info = {}
    return iface_dict


def parse_ethtool_result(lines):
    """
    解析 ethtool 输出信息
    """
    info = {}
    for line in lines:
        split_pos = line.find(":")
        if split_pos == -1:
            continue
        info[line[:split_pos].strip()] = line[split_pos + 1:].strip()
    return info


def parse_lspci_result(lines):
    """
    解析 lspci 输出信息
    """
    info = {}
    for line in lines:
        split_pos = line.find(":")
        if split_pos == -1:
            continue
        info[line[:split_pos].strip()] = line[split_pos + 1:].strip()
    return info


def parse_pci_slot_result(lines):
    """
    解析 dmidecode -t slot 输出信息
    """
    infos = {}
    info = {}
    for line in lines:
        cur_line = line.strip()
        if len(cur_line) == 0:
            bus_info = info.get("Bus Address", None)
            if bus_info is not None:
                infos[bus_info] = info
            info = {}
            continue
        split_pos = cur_line.find(":")
        if split_pos + 1 < len(cur_line):
            info[cur_line[:split_pos].strip()] = cur_line[split_pos + 1:].strip()
    if len(info.keys()) > 0:
        bus_info = info.get("Bus Address", None)
        if bus_info is not None:
            infos[bus_info] = info
    return infos


def get_iface_infos():
    """
    获取 ifconfig 信息
    """
    args = ["ifconfig"]
    p = subprocess.Popen(
        args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)
    try:
        stdout, _ = p.communicate(timeout=3)
    except Exception as e:
        logging.error("wait subprocess finish except: {}".format(e))
        p.terminate()
        sys.exit(1)
    result = stdout.decode("utf-8")
    lines = result.split("\n")
    return parse_ifconfig_result(lines)


def get_ethtool_infos(iface_dict):
    """
    获取 ethtool 相关信息
    :param iface_dict: ifconfig 输出的信息
    """
    iface_eth_dict = {}
    for iface in iface_dict.keys():
        args = ["ethtool", "-i", iface]
        p = subprocess.Popen(
            args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        try:
            stdout, _ = p.communicate(timeout=3)
        except Exception as e:
            logging.error("wait subprocess finish except: {}".format(e))
            p.terminate()
            sys.exit(1)
        result = stdout.decode("utf-8")
        lines = result.split("\n")
        iface_eth_dict[iface] = parse_ethtool_result(lines)
    return iface_eth_dict


def get_pci_infos(iface_eth_dict):
    """
    获取 PCI 相关信息
    :param iface_dict: ethtool -i 输出的信息
    """
    iface_pci_dict = {}
    for iface, eth_info in iface_eth_dict.items():
        bus_info = eth_info.get("bus-info", None)
        if bus_info is None:
            continue
        args = ["lspci", "-vv", "-s", bus_info]
        p = subprocess.Popen(
            args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        try:
            stdout, _ = p.communicate(timeout=3)
        except Exception as e:
            logging.error("wait subprocess finish except: {}".format(e))
            p.terminate()
            sys.exit(1)
        result = stdout.decode("utf-8")
        lines = result.split("\n")
        iface_pci_dict[iface] = parse_lspci_result(lines)
    return iface_pci_dict


def get_pci_slot_infos():
    """
    获取 PCI 插槽信息
    """
    args = ["dmidecode", "-t", "slot"]
    p = subprocess.Popen(
        args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)
    try:
        stdout, _ = p.communicate(timeout=3)
    except Exception as e:
        logging.error("wait subprocess finish except: {}".format(e))
        p.terminate()
        sys.exit(1)
    result = stdout.decode("utf-8")
    lines = result.split("\n")
    return parse_pci_slot_result(lines)


if __name__ == "__main__":
    logging.basicConfig(
        level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

    if not check_commands_exists():
        logging.error("failed check commands")
        sys.exit(1)

    iface_dict = get_iface_infos()
    iface_eth_dict = get_ethtool_infos(iface_dict)
    iface_pci_dict = get_pci_infos(iface_eth_dict)
    bus_slot_dict = get_pci_slot_infos()

    for iface in iface_dict.keys():
        print("----------------")
        print("{}".format(iface))
        iface_info = iface_dict[iface]
        eth_info = iface_eth_dict[iface]
        pci_info = iface_pci_dict.get(iface, {})
        bus_info = eth_info.get("bus-info", None)
        slot_info = bus_slot_dict.get(bus_info, {})
        print("\tipv4: {}".format(iface_info.get("inet", None)))
        print("\tnetmask: {}".format(iface_info.get("netmask", None)))
        print("\tipv6: {}".format(iface_info.get("inet6", None)))
        print("\tbus-info: {}".format(eth_info.get("bus-info", None)))
        print("\tcard name: {}".format(pci_info.get("Subsystem", None)))
        print("\tLnkSta: {}".format(pci_info.get("LnkSta", None)))
        print("\tslot: {}".format(slot_info.get("Designation", None)))
