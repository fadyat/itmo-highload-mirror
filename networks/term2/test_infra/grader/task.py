import abc
import pathlib
import json
import re
import shutil
import subprocess
import codecs
import logging

from typing import Any, Callable

logger = logging.getLogger(__name__)


def copy_sources(
    submit_path: pathlib.Path,
    task_path: pathlib.Path,
    sources: list[str],
    check_fn: Callable[[str], None] | None = None,
) -> None:
    if not submit_path.exists():
        raise RuntimeError("Directory '{}' does not exists".format(submit_path))

    for pattern in sources:
        for file in submit_path.glob(pattern):
            target_path = task_path / file.relative_to(submit_path)
            target_path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(file, target_path)

            if check_fn is not None:
                check_fn(str(target_path))


class TestFailed(Exception):
    pass


class Task:
    @classmethod
    def create(
        cls, name: str, course_unique_commit_id: str, repository_path: pathlib.Path
    ) -> "Task":
        CONTAINERLAB_TASKS = [
            "static_routing",
            "static_routing_backup_link",
            "isis",
            "bgp",
            "bmp",
        ]
        if name in CONTAINERLAB_TASKS:
            import test_infra.grader.containerlab_task as containerlab_task

            return containerlab_task.ContainerlabTask(
                name, course_unique_commit_id, repository_path=repository_path
            )
        else:
            raise ValueError("Unexpected task name '{}'".format(name))

    @classmethod
    def list_tasks(cls, repository_path: pathlib.Path) -> list["Task"]:
        tasks = []
        for path in repository_path.iterdir():
            if not path.is_dir():
                continue

            if not (path / ".tester.json").exists():
                continue

            tasks.append(Task.create(path.name, "list_commit_id", repository_path))

        return tasks

    def __init__(self, name: str, repository_path: pathlib.Path) -> None:
        self.name: str = name

        self.repository_path = repository_path
        self.task_path = self.repository_path / name
        self.task_private_path = self.repository_path / "private" / name

        with (self.task_path / ".tester.json").open() as f:
            self.config = json.load(f)

            self.sources = self.config["allow_change"]
            if not isinstance(self.sources, list):
                self.sources = [self.sources]

            self.regexp_ban = self.config.get("regexp_ban", []) + self.config.get(
                "forbidden_regexp", []
            )
            self.review = self.config.get("review", False)

    def check_call(self, cmd: list[str], **kwargs: Any) -> None:
        logger.info("task check_call of: %s", cmd)
        subprocess.check_call(cmd, **kwargs)

    def check_regexp_ban(self, file: str) -> None:
        try:
            file_content: str = codecs.open(file, encoding="utf-8").read()
        except UnicodeError as e:
            raise RuntimeError("File {} contains non-unicode characters".format(file))

        for regexp in self.regexp_ban:
            if re.search(regexp, file_content, re.MULTILINE):
                raise RuntimeError(
                    "File {} contains banned regexp '{}'".format(file, regexp)
                )

    @abc.abstractmethod
    def grade(self, submit_root: pathlib.Path) -> None:
        raise NotImplementedError("Grade method is not implemented")

    def check(self) -> None:
        for regex in self.regexp_ban:
            re.compile(regex)

        for src in self.sources:
            if not self.task_path.glob(src):
                raise ValueError(
                    "Source file '{}' not found in {}".format(src, self.name)
                )
