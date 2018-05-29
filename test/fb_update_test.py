import requests
import os.path
import re
from requests_oauthlib import OAuth1

position = {'lat': 57.706168,
            'lng': 13.940022}

with open(os.path.dirname(__file__) + '/../private_url.h') as f:

    url = re.match('#define BACKEND_URL \"(.*)\"', f.readline()).group(1)
    # auth = OAuth1(key, secret)

    error_code = requests.post(url, data=None, json=position)
    print(error_code)
