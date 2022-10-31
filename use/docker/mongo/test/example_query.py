import datetime
import time

import pymongo

if __name__ == "__main__":
    client = pymongo.MongoClient("mongodb://root:wsz123@localhost:27017/")
    db = client["examples"]
    collection = db["test"]

    tradingday = int(datetime.datetime.now().strftime("%Y%m%d"))
    # qry = {"tradingday": tradingday, "uid": 5, "oid": 500001}
    # qry = {"uid": 5, "oid": 500001}
    qry = {"tradingday": tradingday, "uid": 5}

    start = time.time()

    docs = collection.find(qry)
    for doc in docs:
        print("{}".format(doc))

    end = time.time()
    print("use elapsed: {}".format(end - start))
