import requests
import os.path
import re
import json


with open(os.path.dirname(__file__) + '/../secrets.h') as f:
    url = re.match('#define BACKEND_URL \"(.*)\"', f.readline()).group(1)
    api_key = re.match('#define API_KEY \"(.*)\"', f.readline()).group(1)
    
    data = {'key': api_key,
            'pos': {
                'lat': 57.706308,
                'lng': 11.940235,
                'prc': 1
                }
            }

    r = requests.post(url, data=json.dumps(data))
    print(r.status_code)
