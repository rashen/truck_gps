import requests
import os.path
from requests_oauthlib import OAuth1

position = {'lat': 57.706168,
            'lng': 11.940022}

with open(os.path.dirname(__file__) + '/../url.key') as f:
    url = f.readline()

    # auth = OAuth1(key, secret)

    error_code = requests.put(url, data=None, json=position)
    print(error_code)
