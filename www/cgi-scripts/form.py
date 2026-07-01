#!/usr/bin/env python3
import os
import sys

method = os.environ.get("REQUEST_METHOD", "GET")

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<form method='POST' action='/cgi-test/form.py'>")
print("<input type='text' name='nombre' placeholder='Tu nombre'>")
print("<input type='submit' value='Enviar'>")
print("</form>")

if method == "POST":
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(length)
    print("<h2>Recibido: " + body + "</h2>")

print("</body></html>")
