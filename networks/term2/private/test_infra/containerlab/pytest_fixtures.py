import pytest
import os

from test_infra.containerlab.network_objects import Host
from test_infra.containerlab.docker_client import DockerClient
from test_infra.containerlab.containerlab_topology import ContainerlabTopology


@pytest.fixture
def workdir(request):
    return request.config.getoption("--workdir")


@pytest.fixture
def task_directory(request):
    return request.config.getoption("--task_directory")


@pytest.fixture
def course_unique_commit_id(request):
    return request.config.getoption("--course_unique_commit_id")


@pytest.fixture()
def clab_topology(workdir, task_directory, topology_file_name, course_unique_commit_id):
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
def docker_client(clab_topology):
    container_name_prefix = f"clab-{clab_topology.get_new_topology_name()}"
    return DockerClient("unix://var/run/docker.sock", container_name_prefix)

