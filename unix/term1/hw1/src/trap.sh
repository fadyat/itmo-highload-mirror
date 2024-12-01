#!/usr/bin/env sh

handle_usr1() {
    date "+%Y-%m-%d %H:%M:%S"
}

handle_usr2() {
    elapsed_time=$(( $(date +%s) - start_time ))
    echo "elapsed $elapsed_time sec"
}

start_time=$(date +%s)
echo "Send USR1, USR2 signals to process with PID: $$"
trap 'handle_usr1' USR1
trap 'handle_usr2' USR2

"$@" &
wait
