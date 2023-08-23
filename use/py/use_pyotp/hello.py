import base64
import pyotp
import time


if __name__ == "__main__":
    key = 'JBSWY3DPEHPK3PXP'
    key_base32 = base64.b32encode(bytes(key, "utf-8"))
    totp = pyotp.TOTP(key_base32.decode("utf-8"))
    t = int(time.time())

    print("key: {}".format(key))
    print("key (base32): {}".format(key_base32.decode("utf-8")))
    print("ts: {}".format(t))
    print("now: {}".format(totp.at(t)))
    print("-30: {}".format(totp.at(t-30)))
    print("+30: {}".format(totp.at(t+30)))
