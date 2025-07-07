#/bin/env bash

set -e -x

black --check --include '\.py_template$' .
black --check .
mypy --explicit-package-bases .
