import datetime

import pymongo

if __name__ == "__main__":
    client = pymongo.MongoClient("mongodb://root:wsz123@localhost:27017/")
    db = client["examples"]
    collection = db["test"]

    # 创建索引
    collection.create_index([
        ("tradingday", pymongo.ASCENDING),
        ("uid", pymongo.ASCENDING),
        ("oid", pymongo.ASCENDING),
    ], background=True, unique=True)

    # 插入数据
    tradingday = int(datetime.datetime.now().strftime("%Y%m%d"))
    for u in range(64):
        docs = []
        uid = u + 1
        for i in range(10000):
            doc = {
                "tradingday": tradingday,
                "uid": uid,
                "oid": uid * 100000 + i,
                "order_price": i + 1,
                "order_vol": i * 100 % 2000,
                "trade_price": i + 1,
                "trade_vol": i * 100 % 200,
                "dt": datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S"),
            }
            docs.append(doc)
        collection.insert_many(docs)
        print("insert uid[{}] docs".format(uid))
