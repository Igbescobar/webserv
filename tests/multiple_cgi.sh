#!/bin/bash

for i in {1..1000}; do
  curl localhost:8080/directory/sleeping.py &
done

wait
