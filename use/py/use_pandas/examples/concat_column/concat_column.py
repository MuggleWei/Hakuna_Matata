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
    # opts, args = getopt.getopt(sys.argv[1:], "d:", ["datas"])
    opts, _ = getopt.getopt(sys.argv[1:], "d:", ["datas"])
    datas_dir = None
    for opt, arg in opts:
        if opt in ("-d", "--datas"):
            datas_dir = arg

    if datas_dir is None:
        print("Usage: {} -d <data dir>".format(sys.argv[0]))
        sys.exit(1)

    return datas_dir


def load_quotes(datas_dir) -> typing.Dict[str, typing.List]:
    """
    load datas
    :param datas_dir: data directory
    """
    k_quotes = {}
    filenames = os.listdir(datas_dir)
    for filename in filenames:
        v = filename.split(".")
        if len(v) != 3:
            continue
        exchange = v[0]
        symbol = v[1]
        file_type = v[2]

        if file_type != "json":
            continue

        k = "{}.{}".format(exchange, symbol)
        filepath = os.path.join(datas_dir, filename)
        with open(filepath, "rb") as f:
            quotes = json.load(f)
        k_quotes[k] = quotes
    return k_quotes


if __name__ == "__main__":
    datas_dir = parse_args()

    # map<k, list<quotes>>
    dict_k_quotes = load_quotes(datas_dir)

    # get all ts
    ts_set = set()
    for k, quotes_list in dict_k_quotes.items():
        for quotes in quotes_list:
            ts_set.add(quotes["ts"])
    ts_list = list(ts_set)
    ts_list.sort()

    # map<k, Dataframe>
    dict_k_df = {}
    for k, quotes_list in dict_k_quotes.items():
        df = pd.DataFrame(quotes_list)
        df.set_index("ts", inplace=True)
        print("------------------------------------------------")
        print("{}\n{}\n".format(k, df))
        df = df.reindex(ts_list, copy=False)
        print("{} after reindex\n{}\n".format(k, df))
        df.sort_index(ascending=True, inplace=True)
        # fillup na with prev
        df.fillna(method='ffill', inplace=True)
        # fillup na with 0
        df.fillna(0.0, inplace=True)
        print("{} after fillna\n{}\n".format(k, df))
        print("------------------------------------------------")
        dict_k_df[k] = df

    # get all close dataframe
    keys = []
    close = []
    for k, df in dict_k_df.items():
        keys.append(k)
        close.append(df["close"])
    df_close = pd.concat(close, axis=1, keys=keys)
    print("------------------------------------------------\n"
          "all close price\n{}\n"
          "------------------------------------------------\n"
          "".format(df_close))

    # print all column
    print(df_close.index)
    print("------------------------------------------------\n"
          "index\n{}\n"
          "------------------------------------------------\n"
          "".format(df_close.index))
    for k in dict_k_quotes.keys():
        print("------------------------------------------------\n"
              "{}\n{}\n"
              "------------------------------------------------\n"
              "".format(k, df_close[k]))
