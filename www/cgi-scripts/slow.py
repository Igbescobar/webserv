#!/usr/bin/env python3
import time
import os

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<p>Empezando proceso lento...</p>")

time.sleep(5)  # simula trabajo pesado

print("<p>Terminado después de 5 segundos</p>")
print("</body></html>")
