import requests


class Spam(object):

    """Spam class"""

    def get_ip(self):
        """get ip address

        :returns: ip address

        """
        ip = None
        try:
            res = requests.get("https://httpbin.org/ip")
            ip = res.json()["origin"]
        except Exception as e:
            print("failed get ip: {}".format(e))
        return ip
