#!/usr/bin/env python3
import os
import datetime
import socket
import subprocess

hostname = socket.gethostname()
now = datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC")
uptime = subprocess.check_output(["uptime", "-p"]).decode().strip()

print("Content-Type: text/html")
print("")
print("<html><body>")
print("<h1>CGI execution verified</h1>")
print("<table border='1' cellpadding='6'>")
print("<tr><th>Field</th><th>Value</th></tr>")
print("<tr><td>Timestamp</td><td>" + now + "</td></tr>")
print("<tr><td>Host</td><td>" + hostname + "</td></tr>")
print("<tr><td>Uptime</td><td>" + uptime + "</td></tr>")
print("<tr><td>PID</td><td>" + str(os.getpid()) + "</td></tr>")
print("<tr><td>Method</td><td>" + os.environ.get("REQUEST_METHOD", "") + "</td></tr>")
print("<tr><td>Query</td><td>" + os.environ.get("QUERY_STRING", "") + "</td></tr>")
print("<tr><td>Script</td><td>" + os.environ.get("SCRIPT_FILENAME", "") + "</td></tr>")
print("</table>")
print("</body></html>")
