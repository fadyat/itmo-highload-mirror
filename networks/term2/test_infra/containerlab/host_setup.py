#!/bin/env python3

import logging
from pathlib import Path

from test_infra.containerlab.network_objects import Host
from test_infra.containerlab.docker_client import DockerClient

logger = logging.getLogger(__name__)


def copy_file_to_container(
    client: DockerClient, task_directory: Path, hostname: str, filename: str
) -> None:
    logging.info("copy file %s to host %s", filename, hostname)
    with open(task_directory / filename, "rb") as solution_file:
        solution_data = solution_file.read()
        client.put_file(hostname, "/", filename, solution_data)


def run_host_setup(
    client: DockerClient,
    task_directory: Path,
    hosts: list[Host],
    setup_agent_file_name: str = "main.py",
) -> None:
    for host in hosts:
        copy_file_to_container(client, task_directory, host.name, setup_agent_file_name)

    for host in hosts:
        logging.info("run setup for host %s", host.name)
        output = client.execute_command_in_container(
            host.name, ["python3", f"/{setup_agent_file_name}"]
        )
        logging.info("setup script output:\n%s", output)
