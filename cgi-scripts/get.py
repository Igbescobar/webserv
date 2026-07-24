#!/usr/bin/python3

import os

msg = "hello from python cgi\n"

msg +=  "TERM is "
term = os.getenv("TERM")
if term:
    msg += term + "\n"
else:
    msg += "not found\n"

msg +=  "REQUEST_METHOD is "
method = os.getenv("REQUEST_METHOD")
if method:
    msg += method + "\n"
else:
    msg += "not found\n"

print("Content-Type: text/plain")
print(f"Content-Length: {len(msg) + 1}")
print()
print(msg)
