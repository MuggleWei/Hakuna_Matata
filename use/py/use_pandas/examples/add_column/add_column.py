import getopt
import json
import os
import sys
import typing

import pandas as pd


def parse_args():
    """
    parse arguments
    """
    opts, _ = getopt.getopt(sys.argv[1:], "f:", ["file"])
    filepath = None
    for opt, arg in opts:
        if opt in ("-f", "--file"):
            filepath = arg

    if filepath is None:
        print("Usage: {} -f <data file>".format(sys.argv[0]))
        sys.exit(1)

    return filepath


def load_file(filepath) -> typing.List:
    """
    read data file
    :param filepath: data file path
    """
    filename = os.path.basename(filepath)
    v = filename.split(".")
    if len(v) != 3:
        raise Exception("invalid file name: {}".format(filename))

    file_type = v[2]
    if file_type != "json":
        raise Exception("invalid file type: {}".format(filename))

    with open(filepath, "rb") as f:
        quotes = json.load(f)
    for row in quotes:
        row["open"] = float(row["open"])
        row["high"] = float(row["high"])
        row["low"] = float(row["low"])
        row["close"] = float(row["close"])
        row["volume"] = int(row["volume"])

    return quotes


if __name__ == "__main__":
    filepath = parse_args()

    # load data file
    quotes = load_file(filepath)
    df = pd.DataFrame(quotes)
    df.set_index("ts", inplace=True)
    print(df)

    # add column: MA5
    row, col = df.shape
    close = df["close"]
    ma = []
    r = 5
    queue = []
    s = 0
    for close_price in close:
        queue.append(close_price)
        s += close_price
        len_queue = len(queue)
        if len_queue > r:
            s -= queue[0]
            queue.pop(0)
            ma.append(s / r)
        elif len_queue == r:
            ma.append(s / r)
        else:
            ma.append(None)
    df["MA5"] = ma

    print(df)
