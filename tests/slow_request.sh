#!/usr/bin/env bash

TARGET="localhost"
PORT=8080
TIME=1

send_request() {
# Using a subshell to group the output for the pipe
	{
		printf "GET / HTTP/1.1\r\n"
			sleep $TIME
			printf "Host: %s\r\n" "$TARGET"
			sleep $TIME
			printf "Connection: close"
			sleep $TIME
			printf "\r\n\r\n"
			sleep $TIME
			printf "Optional Body Content\n"
			sleep $TIME
	} | nc "$TARGET" "$PORT"
}

send_request
