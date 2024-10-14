#!/usr/bin/env bash

# Some useful commands to prepare the environment
# Note: you don't need to run this script, it's just a reference
#
# https://github.com/wldhx/yadisk-direct

ssh -i ~/.ssh/vvfadeev root@87.228.17.23
curl -L "$(yadisk-direct https://disk.yandex.ru/d/mgMoFqRka9DdLA)" -o /tmp/yandex_trips.csv
psql postgresql://postgres:postgres@87.228.17.23:5432/postgres