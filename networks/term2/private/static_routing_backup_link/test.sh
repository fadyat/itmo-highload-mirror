#/bin/env bash

set -e -x

if [[ -z "${REPO}" ]]; then
   echo REPO path is missing
   exit 1
fi

if [[ -z "${WORKDIR}" ]]; then
   echo WORKDIR path is missing
   exit 1
fi

# Enter virtual environment.
source ${WORKDIR}/venv/bin/activate

# This script is in the root of the task. 
# As result dirname of file executable is what we need.
TASKDIR=$(dirname "$(readlink -f "$0")")

# Run checker script.
# --capture=no - disable stdin/stdout capturing. This fixes stdout of child processes in terminal.
# -o log_cli=true - enable live logging to terminal
# -o log_cli_level = "DEBUG" - just logging verbosity level
# -p no:cacheprovider - to disabling cache, as there is no chance that the test ran before.
# -p test_infra.containerlab.pytest_hooks - module with custom pytest hooks with parametrization by topology file and command line arguments
# -p test_infra.containerlab.pytest_fixtures - module with pytest fixtures

PYTHONPATH=${REPO} pytest \
    --capture=no \
    -o log_cli=true \
    -o log_cli_level="INFO" \
    -p no:cacheprovider \
    -p test_infra.containerlab.pytest_hooks \
    -p test_infra.containerlab.pytest_fixtures \
    ${TASKDIR} \
    --task_directory=${TASKDIR} \
    --topology_file_name=lab.yml \
    --workdir=${WORKDIR} \
    --course_unique_commit_id=${COURSE_UNIQUE_COMMIT_ID}
