#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/plain")
print("")
print("METHOD: " + os.environ.get("REQUEST_METHOD", ""))
print("QUERY: " + os.environ.get("QUERY_STRING", ""))
print("SCRIPT: " + os.environ.get("SCRIPT_FILENAME", ""))
