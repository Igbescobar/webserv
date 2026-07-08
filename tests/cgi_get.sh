#!/bin/bash

TARGET="localhost"
PORT=8080

send_request_python() {
    # Using a subshell to group the output for the pipe
    {
        printf "GET /directory/get.py HTTP/1.1\r\n"
        printf "Host: %s:%s\r\n" "$TARGET" "$PORT"
        printf "Connection: close\r\n"
        printf "\r\n" # The mandatory empty line separating headers and body
    } | nc "$TARGET" "$PORT"
}

# send_request_php() {
#     # Using a subshell to group the output for the pipe
#     {
#         printf "GET /directory/get.php HTTP/1.1\r\n"
#         printf "Host: %s:%s\r\n" "$TARGET" "$PORT"
#         printf "Connection: close\r\n"
#         printf "\r\n" # The mandatory empty line separating headers and body
#     } | nc "$TARGET" "$PORT"
# }

printf "=== python3 ===\n"
send_request_python

# printf "=== php ===\n"
# send_request_php
