#!/usr/bin/env python3
import sys

# The HTTP header is mandatory for CGI scripts so the server knows how to handle the data
print("Content-Type: text/plain\r\n")

file_path = "test/test.txt"-

try:
    with open(file_path, 'r') as file:
        content = file.read()
        # Print the contents of the file directly to stdout
        print(content)
except FileNotFoundError:
    print(f"Error: The file '{file_path}' does not exist.")
except PermissionError:
    print(f"Error: You do not have permission to read '{file_path}'.")
except Exception as e:
    print(f"An unexpected error occurred: {e}")
