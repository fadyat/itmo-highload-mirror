#!/bin/env python3

import logging
from pathlib import Path

from test_infra.containerlab.network_objects import Host
from test_infra.containerlab.docker_client import DockerClient

logger = logging.getLogger(__name__)

def run_host_setup(
    client: DockerClient, task_directory: Path, hosts: list[Host]
):
    with open(task_directory / "main.py", "rb") as solution_file:
        solution_data = solution_file.read()
        for host in hosts:
            logging.info("run setup for host %s", host.name)
            client.put_file(host.name, "/", "main.py", solution_data)
            output = client.execute_command_in_container(
                host.name, ["python3", "/main.py"]
            )
            logging.info("setup script output:\n%s", output)
            