#!/usr/bin/env bash

wget -S $1 2>&1 | grep -q -m 1 "Expires: Sun, 19 Nov 1978 05:00:00 GMT" && echo "Yes" || echo "No"
