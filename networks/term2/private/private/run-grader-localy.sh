#/bin/env bash

set -e -x

if [[ -z "${TASK_NAME}" ]]; then
   echo TASK_NAME is not set.
   echo TASK_NAME should be directory name with the task, e.g. static_routing.
   echo Exiting...
   exit 1
fi

# This script is in the root of the directory. 
# As result dirname of file executable is what we need.
REPOSITORY_ROOT=$(dirname "$(readlink -f "$0")")

# Set fake gitlab ci environment variables.
export CI_COMMIT_REF_NAME="submit/${TASK_NAME}"
export GITLAB_USER_LOGIN="test-user"
export GITLAB_USER_ID="test-user_id"
export CI_COMMIT_SHORT_SHA="12345"
export CI_PROJECT_DIR=${REPOSITORY_ROOT}

# SKIP_TOPOLOGY_CLEANUP=1 keeps topology up and running after the last test
# useful for tests debugging.
export SKIP_TOPOLOGY_CLEANUP="1"

# SKIP_REPORT=1 configures grader to not send report to Manytask, 
# as this script is for local runs, no reason to try to report anything.
export SKIP_REPORT="1"

# All scripts in the project use absolute imports from root of repository.
# As result we need to run them with pythonpath pointing to the root.
PYTHONPATH=${REPOSITORY_ROOT} python3 test_infra/grader/__main__.py
