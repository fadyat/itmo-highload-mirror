#!/bin/bash

failed_tests=()

print_status_and_record () {
  local test_name=$1
  local status=$2
  if [ $status -ne 0 ]; then
    failed_tests+=("$test_name")
    echo FAILED
  else
    echo SUCCESS
  fi
  echo
}

test_must_compile_and_run () {
  echo "*** Данный тест должен успешно скомпилироваться и запуститься: $1"
  clang++ \
    -std=c++20 \
    -Wall -Wextra -Werror -Wextra-semi -Wno-self-assign-overloaded -fno-operator-names \
    -O1 -g -fsanitize=address -fno-omit-frame-pointer \
    -iquote. \
    -o /tmp/out \
    $1 && \
  /tmp/out 
  local result=$?
  print_status_and_record "test_must_compile_and_run $1" $result
  return $result
}

test_must_compile () {
  echo "*** Данный тест должен успешно скомпилироваться (но может упасть при запуске): $1"
  clang++ \
    -std=c++20 \
    -Wall -Wextra -Werror -Wextra-semi -Wno-self-assign-overloaded -fno-operator-names \
    -iquote. \
    -o /tmp/out \
    $1
  local result=$?
  print_status_and_record "test_must_compile $1" $result
  return $result
}

test_must_not_compile () {
  echo "*** Данный тест НЕ ДОЛЖЕН скомпилироваться: $1"
  ! clang++ \
    -std=c++20 \
    -Wall -Wextra \
    -iquote. \
    -o /tmp/out \
    $1 2> /dev/null
  local result=$?
  print_status_and_record "test_must_not_compile $1" $result
  return $result
}

if test_must_compile_and_run smoke_test.cpp; then
  echo "Smoke tests passed, running rest of the tests"
  for f in tests/minimal/run_*.cpp; do
    test_must_compile_and_run "$f"
  done
  for f in tests/main/comp_*.cpp; do
    test_must_compile "$f"
  done
  for f in tests/main/ncomp_*.cpp; do
    test_must_not_compile "$f"
  done
  for f in tests/main/run_*.cpp; do
    test_must_compile_and_run "$f"
  done
fi

if [ ${#failed_tests[@]} -eq 0 ]; then
  echo "All tests succeeded!"
else
  echo "${#failed_tests[@]} tests failed:"
  (IFS=$'\n'; echo "${failed_tests[*]}")
  exit 1
fi
