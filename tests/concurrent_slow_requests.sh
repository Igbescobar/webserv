#!/usr/bin/env bash

for i in {1..100}; do
	./slow_request.sh &
done

wait
