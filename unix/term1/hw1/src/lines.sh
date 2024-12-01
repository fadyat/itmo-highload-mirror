#!/usr/bin/env sh

find . -type f -exec wc -l {} + | tail -n 1 | awk '{ print $1 }'
