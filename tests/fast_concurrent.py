#!/usr/bin/env python3

import threading
import requests

# The URL of your server from your default.conf (e.g., localhost:8080)
URL = "http://localhost:8080/"
CONCURRENT_REQUESTS = 50

def send_request():
    try:
        response = requests.get(URL)
        print(f"Status: {response.status_code}")
    except Exception as e:
        print(f"Request failed: {e}")

threads = []
for i in range(CONCURRENT_REQUESTS):
    t = threading.Thread(target=send_request)
    threads.append(t)
    t.start()

for t in threads:
    t.join()
