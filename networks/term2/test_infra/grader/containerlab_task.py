import os
import pathlib

import test_infra.grader.task as task

CONTAINERLAB_IMAGE_TAG = (
    "gitlab.manytask.org:5050/computer-networks/public-2025-spring/containerlab:latest"
)


class ContainerlabTask(task.Task):
    def __init__(
        self, name: str, course_unique_commit_id: str, repository_path: pathlib.Path
    ):
        super().__init__(name, repository_path)
        self.course_unique_commit_id: str = course_unique_commit_id

    def grade(self, repository_path: pathlib.Path) -> None:
        docker_repository_mount_path = pathlib.Path("/opt/shad")
        # fmt: off
        # Black makes this list unreadable.
        cmd = [
                "docker", "run", "-t", "--rm", "--privileged",
                "--network", "host",
                "-v", "/var/run/docker.sock:/var/run/docker.sock",
                "-v", "/var/run/netns:/var/run/netns",
                "-v", "/etc/hosts:/etc/hosts",
                "-v", "/var/lib/docker/containers:/var/lib/docker/containers",
                "--pid=host",
                "-e", "WORKDIR=/workdir",
                "-e", f"COURSE_UNIQUE_COMMIT_ID={self.course_unique_commit_id}",
                "-e", f"REPO={docker_repository_mount_path}",
                "-e", f"SKIP_TOPOLOGY_CLEANUP={os.environ.get('SKIP_TOPOLOGY_CLEANUP', '')}",
                "-v", f"{repository_path}:{docker_repository_mount_path}:ro",
                "-w", "/workdir",
                CONTAINERLAB_IMAGE_TAG, "bash", f"{docker_repository_mount_path / self.name}/test.sh"
        ]
        # fmt: on

        self.check_call(cmd)
