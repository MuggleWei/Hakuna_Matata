import time

import pymongo

if __name__ == "__main__":
    client = pymongo.MongoClient("mongodb://root:wsz123@localhost:27017/")
    db = client["examples"]
    collection = db["test"]

    qry = {}
    new_values = {"$set": {"is_valid": 1}}

    start = time.time()

    collection.update_many(qry, new_values)

    end = time.time()
    print("use elapsed: {}".format(end - start))