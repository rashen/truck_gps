import requests
import os.path
import re

position = {'lat': 57.706309,
            'lng': 11.940235}

with open(os.path.dirname(__file__) + '/../private_url.h') as f:
    with open(os.path.dirname(__file__) + '/../private_api_key.h') as g:
        url = re.match('#define BACKEND_URL \"(.*)\"', f.readline()).group(1)
        api_key = re.match('#define API_KEY \"(.*)\"', g.readline()).group(1)
        
        key = {'key': api_key}

        r = requests.post(url, json={**key, **position})
        print(r.status_code)
