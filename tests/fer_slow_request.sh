#!/bin/bash

(
  # Request line
  printf "POST "
  sleep 1
  printf "/ HTTP/1.0\r\n"
  sleep 1
  # Headers uno por uno
  printf "Host: localhost:8080\r\n"
  sleep 1
  printf "Transfer-Enc"
  sleep 1
  printf "oding: chunked"
  sleep 1
  printf "\r\n"
  sleep 1
  printf "\r\n"
  sleep 1

  # Chunk 1: "Hello" (5 bytes = 0x5)
  printf "5\r\n"
  sleep 1
  printf "Hello"
  sleep 1
  printf "\r\n"
  sleep 1

  # Chunk 2: "World!" (6 bytes = 0x6)
  printf "6\r\n"
  sleep 1
  printf "World!"
  sleep 1
  printf "\r\n"
  sleep 1

  # Chunk final
  printf "0\r\n"
  sleep 1
  printf "\r\n"

) | nc localhost 8080
