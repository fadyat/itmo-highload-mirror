#!/bin/env python3

from pathlib import Path

import io
import concurrent
import docker
import logging
import tarfile


logger = logging.getLogger(__name__)


def create_tar_from_bytes(filename, file_data):
    """
    Create a tar archive from bytes data.
    """
    # Create an in-memory bytes buffer
    tar_buffer = io.BytesIO()

    # Open the buffer as a tarfile for writing
    with tarfile.open(fileobj=tar_buffer, mode="w") as tar:
        # Create a TarInfo object for each file
        tar_info = tarfile.TarInfo(name=filename)
        tar_info.size = len(file_data)

        # Add the file to the tar archive
        tar.addfile(tarinfo=tar_info, fileobj=io.BytesIO(file_data))

    # Get the tar archive bytes
    tar_buffer.seek(0)
    return tar_buffer.read()


class DockerClient:
    """Wrapper of docker client convenient for test purposes."""

    # Tests can execute many commands in parallel. Set pool size
    # to some large enough number.
    DOCKER_CONNECTION_POOL_SIZE = 300

    def __init__(self, docker_socket: str, container_name_prefix: str):
        self.client = docker.DockerClient(
            base_url=docker_socket, max_pool_size=self.DOCKER_CONNECTION_POOL_SIZE
        )
        self.prefix = container_name_prefix

    def execute_command_in_container(self, host_name: str, command: str | list[str]):
        """
        Execute a command in a specified Docker container using the Docker SDK.
        Returns output of the command, or raises exception in case of error.

        :param host_name: Name of the host in the topology.
        :param command: Command to execute in the container.
        :return: Output of the command.
        """
        container_name = f"{self.prefix}-{host_name}"
        container = self.client.containers.get(container_name)
        logger.debug("Executing %s in container %s", command, container_name)
        result = container.exec_run(command, stdout=True, stderr=True, tty=True)

        if result.exit_code != 0:
            logger.error(
                f"Error in container {container_name}\n command: {command}\noutput: {result.output.decode().strip()}"
            )
            raise RuntimeError(f"Failed to execute command in container {container_name}")
        else:
            logger.debug(
                f"Command executed in container {container_name}\n command: {command}\noutput: {result.output.decode().strip()}"
            )

        return result.output.decode().strip()

    def put_file(self, host_name: str, directory: str, file_name: str, data: bytes):
        """
        Puts file into Docker container using the Docker SDK.

        :param host_name: Name of the host in the topology.
        :param command: Command to execute in the container.
        :return: Output of the command.
        """
        container_name = f"{self.prefix}-{host_name}"
        container = self.client.containers.get(container_name)
        container.put_archive(directory, create_tar_from_bytes(file_name, data))