import pytest
import os
from typing import Generator
from pathlib import Path

from test_infra.containerlab.network_objects import Host
from test_infra.containerlab.docker_client import DockerClient
from test_infra.containerlab.containerlab_topology import ContainerlabTopology


@pytest.fixture
def workdir(request: pytest.FixtureRequest) -> Path:
    return Path(request.config.getoption("--workdir"))


@pytest.fixture
def task_directory(request: pytest.FixtureRequest) -> Path:
    return Path(request.config.getoption("--task_directory"))


@pytest.fixture
def course_unique_commit_id(request: pytest.FixtureRequest) -> str:
    return str(request.config.getoption("--course_unique_commit_id"))


@pytest.fixture()
def clab_topology(
    workdir: Path,
    task_directory: Path,
    topology_file_name: str,
    course_unique_commit_id: str,
) -> Generator[ContainerlabTopology, None, None]:
    topology = ContainerlabTopology(
        workdir,
        task_directory,
        topology_file_name,
        course_unique_commit_id,
    )
    topology.setup_topology_file()
    topology.start()
    yield topology
    if not os.environ.get("SKIP_TOPOLOGY_CLEANUP", ""):
        topology.cleanup()


@pytest.fixture()
def docker_client(clab_topology: ContainerlabTopology) -> DockerClient:
    container_name_prefix = f"clab-{clab_topology.get_new_topology_name()}"
    return DockerClient("unix://var/run/docker.sock", container_name_prefix)
