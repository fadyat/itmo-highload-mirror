#!/usr/bin/env bash
set -uo pipefail

ALL="postgres mongo neo4j spark"

usage() {
  echo "Usage: $0 {up|down|check} [${ALL// /|}|all]..."
  exit 1
}

[[ $# -lt 1 ]] && usage

action="$1"; shift
targets="$*"
[[ -z "$targets" || "$targets" == "all" ]] && targets="$ALL"

up()   { docker compose -f "$1/docker-compose.yml" up -d; }
down() { docker compose -f "$1/docker-compose.yml" down -v --rmi all --remove-orphans; }

check() {
  local t="$1" c ext cmd q out
  case "$t" in
    postgres) c=hw0-postgres; ext=sql;         cmd="psql -U admin -d university -v ON_ERROR_STOP=1 -f" ;;
    mongo)    c=hw0-mongo;    ext=mongodb.js;  cmd="mongosh --quiet shop --file" ;;
    neo4j)    c=hw0-neo4j;    ext=cypher;      cmd="cypher-shell -u neo4j -p password -f" ;;
    spark)    c=hw0-spark;    ext=sql;         cmd="/opt/spark/bin/spark-sql -f" ;;
    *)        echo "unknown: $t" >&2; return 1 ;;
  esac
  for q in q1 q2 q3; do
    # shellcheck disable=SC2086
    if out=$(docker exec "$c" $cmd "/queries/$q.$ext" 2>&1); then
      echo "[PASS] $t/$q"; ((pass++))
    else
      echo "[FAIL] $t/$q"
      echo "       ${out//$'\n'/$'\n'       }"
      ((fail++))
    fi
  done
}

pass=0 fail=0
for t in $targets; do
  case "$action" in
    up|down) echo "==> $t: $action"; "$action" "$t" ;;
    check)   check "$t" ;;
    *)       usage ;;
  esac
done

if [[ "$action" == "check" ]]; then
  echo "Passed: $pass  Failed: $fail"
  [[ $fail -eq 0 ]]
fi
