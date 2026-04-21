#!/bin/bash

for i in {1..2000}; do
	./slow_request.sh &
done

wait
