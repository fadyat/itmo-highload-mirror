#!/usr/bin/env bash
set -uo pipefail

cd "$(dirname "$0")"

usage() {
  echo "Usage: $0 {up|down|run <01|02|03|04|05>}"
  exit 1
}

[[ $# -lt 1 ]] && usage
action="$1"; shift

case "$action" in
  up)   docker compose up -d ;;
  down) docker compose down -v --remove-orphans ;;
  run)
    [[ $# -lt 1 ]] && usage
    app=$(ls spark/apps | grep "^${1}_" | head -n1)
    [[ -z "$app" ]] && { echo "no app matching $1"; exit 1; }
    docker exec hw1-spark /opt/spark/bin/spark-submit "/apps/$app"
    ;;
  *) usage ;;
esac
