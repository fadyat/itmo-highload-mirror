#!/usr/bin/env bash
set -uo pipefail

cd "$(dirname "$0")"

usage() {
  cat <<EOF
Usage: $0 <command> [args]

Commands:
  up [profile...]     docker compose up -d (default: all), then attach to logs. Ctrl-C detaches.
  down                docker compose down -v --remove-orphans
  logs <service>      tail logs of a service
  seed <N>            POST N random users to the API
  status              show row counts in postgres / kafka / es / cassandra / mongo
EOF
  exit 1
}

[[ $# -lt 1 ]] && usage
action="$1"; shift

compose_with_profiles() {
  local args=()
  if [[ $# -eq 0 ]]; then
    args+=(--profile all)
  else
    for p in "$@"; do args+=(--profile "$p"); done
  fi
  docker compose "${args[@]}" up -d
  exec docker compose "${args[@]}" logs -f
}

case "$action" in
  up)
    compose_with_profiles "$@"
    ;;
  down)
    docker compose --profile all down -v --remove-orphans
    ;;
  logs)
    [[ $# -lt 1 ]] && usage
    docker compose logs -f "$1"
    ;;
  seed)
    [[ $# -lt 1 ]] && usage
    n="$1"
    countries=(RU US DE FR GB JP CN BR IN ES)
    for i in $(seq 1 "$n"); do
      country="${countries[$((RANDOM % ${#countries[@]}))]}"
      ts=$(date +%s%N)
      curl -sS -X POST http://localhost:8080/users \
        -H 'Content-Type: application/json' \
        -d "{\"name\":\"user-$i\",\"email\":\"u${i}-${ts}@example.com\",\"country\":\"$country\"}" \
        > /dev/null && echo -n "."
    done
    echo
    ;;
  status)
    printf '%-15s %s\n' \
      "postgres"      "$(docker exec hw2-postgres psql -U admin -d app -tAc "select count(*) from users" 2>/dev/null)" \
      "jsonl files"   "$(ls data/users-*.jsonl 2>/dev/null | wc -l | tr -d ' ')" \
      "kafka topic"   "$(docker exec hw2-kafka1 /opt/kafka/bin/kafka-get-offsets.sh --bootstrap-server kafka1:9092 --topic users 2>/dev/null | awk -F: '{s+=$3} END {print s+0}')" \
      "elasticsearch" "$(docker exec hw2-elasticsearch curl -sS http://localhost:9200/users/_count 2>/dev/null | grep -oE '"count":[0-9]+' | cut -d: -f2)" \
      "cassandra"     "$(docker exec hw2-cassandra cqlsh -e "select count(*) from app.users" 2>/dev/null | awk '/^[[:space:]]+[0-9]+/ {print $1; exit}')" \
      "mongo"         "$(docker exec hw2-mongo mongosh --quiet app --eval 'db.users.countDocuments({})' 2>/dev/null | tail -1 | tr -d '\r')"
    ;;
  *) usage ;;
esac
