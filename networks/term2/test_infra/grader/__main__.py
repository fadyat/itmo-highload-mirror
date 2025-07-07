import requests
import time
import os
import pathlib
import logging

from test_infra.grader.task import Task, TestFailed

# Configure logging
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)


def push_report(user_id: str, user_name: str, task: str, failed: bool = False) -> None:
    """Sends score to manytask."""
    url = "https://networks.manytask.org/api/report"

    logging.info(
        "Try to report score for user_id=%s user_name=%s task=%s failed=%s",
        user_id,
        user_name,
        task,
        failed,
    )
    # Do not expose token in logs.
    for _ in range(3):
        data: dict[str, str | int] = {
            "token": os.environ["TESTER_TOKEN"],
            "task": task,
            "username": user_name,
        }

        if failed:
            data["failed"] = 1

        rsp = requests.post(url, data=data)

        if rsp.status_code != 500 or failed:
            break
        else:
            time.sleep(1.0)

    rsp.raise_for_status()


def main() -> None:
    # Extract arguments from gitlab CI environment variables.
    task_name = os.environ["CI_COMMIT_REF_NAME"].split("/")[1]
    repository_path = os.environ["CI_PROJECT_DIR"]
    user_id = os.environ["GITLAB_USER_ID"]
    user_name = os.environ["GITLAB_USER_LOGIN"]
    commit_sha_short = os.environ["CI_COMMIT_SHORT_SHA"]
    course_unique_commit_id = f"{user_name}-{commit_sha_short}"
    skip_report = "SKIP_REPORT" in os.environ

    task = Task.create(
        task_name, course_unique_commit_id, pathlib.Path(repository_path)
    )
    try:
        logging.info("Start grading of %s", task_name)
        task.grade(pathlib.Path(repository_path))

        if task.review or skip_report:
            logging.info("Skipping report of score")
            return

        logging.info("Reporting score")
        push_report(user_id, user_name, task_name, failed=False)
    except TestFailed:
        logging.info("Reporting test failed")
        push_report(user_id, user_name, task_name, failed=True)
        raise


if __name__ == "__main__":
    main()
