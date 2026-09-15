import time

import psutil


def get_core_count():
    """
    获取 CPU 核心数量(包括逻辑核心和无力核心)
    """
    n_core_logical = psutil.cpu_count(logical=True)
    n_core_physical = psutil.cpu_count(logical=False)
    return n_core_logical, n_core_physical


def get_core_temps():
    """
    获取温度
    :return:
        - 当成功时, 返回 传感器名称, [(标签, 温度), ...]
        - 当失败时, 返回 None, []
    :NOTE:
        - coretemp：Intel CPU 各核心温度
        - k10temp：AMD CPU 温度
        - acpitz：ACPI 热区，通常是主板/环境温度
        - nvme：部分内核版本下 NVMe 温度会出现在这里
        - jc42：内存 DIMM 温度（部分主板）
        - dell_smm、hp、thinkpad：厂商专用传感器
        - amdgpu、nouveau、nvidia：GPU 温度（如果有）
    """
    temps = psutil.sensors_temperatures()
    if not temps:
        return None, []

    # 按优先级找 CPU 相关传感器
    for name in ("coretemp", "k10temp", "cpu_thermal", "acpitz"):
        if name in temps:
            entries = [(e.label or "unknown", e.current) for e in temps[name]]
            return name, entries

    # 都没匹配到, 返回第一个可用的
    name = next(iter(temps))
    entries = [(e.label or "unknown", e.current) for e in temps[name]]
    return name, entries


if __name__ == "__main__":
    # 核心数
    n_core_logical, n_core_physical = get_core_count()
    print("-" * 50)
    print("CPU 核心数量")
    print("  逻辑核心: {}".format(n_core_logical))
    print("  物理核心: {}".format(n_core_physical))

    # 温度
    print("-" * 50)
    print("CPU 温度")
    sensor_name, temps = get_core_temps()
    if sensor_name is None:
        print("  无法读取温度 (可能缺少驱动或在虚拟机中)")
    else:
        print("  传感器: {}".format(sensor_name))
        for label, celsius in temps:
            print("  {:>12}: {:6.1f}".format(label, celsius))

    # 频率
    print("-" * 50)
    print("CPU 频率")
    freqs = psutil.cpu_freq(percpu=True)
    if not freqs:
        print("  无法读取频率")
    else:
        for i, f in enumerate(freqs):
            print("  CPU{:<3}: {:7.0f}MHz  (最小 {:6.0f}MHz / 最大 {:6.0f}MHz)"\
                  .format(i, f.current, f.min, f.max))

    # 使用率
    # 第一次用于建立基线, 结果是 0.0, 需要丢弃
    psutil.cpu_percent(percpu=True)
    time.sleep(1.0)

    percents = psutil.cpu_percent(percpu=True)
    print("-" * 50)
    print("CPU 使用率(1 秒采样)")
    for i, p in enumerate(percents):
        print("  CPU{:<3} {:5.1f}%".format(i, p))
    print("  总体  {:5.1f}".format(psutil.cpu_percent()))
