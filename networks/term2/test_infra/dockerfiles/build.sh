#/bin/env bash

set -e -x

docker build -f testenv.docker -t testenv .
docker build -f containerlab.docker -t containerlab .
docker build -f frrenv.docker -t frrenv .