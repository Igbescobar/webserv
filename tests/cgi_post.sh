#!/bin/bash

TARGET="localhost"
PORT=8080

send_request() {
    # Define the body payload
    BODY="field1=value1&field2=value2"

    # Calculate the exact byte length of the body
    CONTENT_LENGTH=${#BODY}

    # Using a subshell to group the output for the pipe
    {
        printf "POST /directory/cgi_post.py HTTP/1.1\r\n"
        printf "Host: %s:%s\r\n" "$TARGET" "$PORT"
        printf "Connection: close\r\n"
        printf "Content-Type: application/x-www-form-urlencoded\r\n"
        printf "Content-Length: %d\r\n" "$CONTENT_LENGTH"
        printf "\r\n" # The mandatory empty line separating headers and body
        printf "%s" "$BODY"
    } | nc "$TARGET" "$PORT"
}

send_request
