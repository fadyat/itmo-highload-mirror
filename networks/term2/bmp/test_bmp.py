#!/bin/env python3

import logging
import time
import pytest

import bmp.network_topology as network_topology

from pathlib import Path
from ipaddress import IPv4Address
from typing import Generator

from test_infra.containerlab.docker_client import DockerClient
from test_infra.containerlab.host_setup import run_host_setup, copy_file_to_container

logger = logging.getLogger(__name__)

MONITORING_HOST_NAME = "monitoring-client"
SPEAKER_HOST = "speaker"
SPEKER_PORT = 17957
CLIENT_LOG_FILE_NAME = "bmp_client.log"


def start_bmp_client(
    client: DockerClient,
    task_directory: Path,
    hostname: str,
    bmp_client_file_name: str,
    log_file_name: str,
) -> None:
    copy_file_to_container(client, task_directory, hostname, bmp_client_file_name)

    logging.info("start bmp client at host %s", hostname)
    client.execute_command_in_container(
        hostname,
        [
            "python3",
            f"/{bmp_client_file_name}",
            "--bmp-ip-addr",
            "10.0.0.0",
            "--bmp-port",
            "11019",
            "--stats-server-ip",
            "127.0.0.1",
            "--stats-server-port",
            str(SPEKER_PORT),
            "--log-file",
            log_file_name,
        ],
        detach=True,
    )
    time.sleep(1)


@pytest.fixture(autouse=True)
def setup_fixture(
    docker_client: DockerClient, task_directory: Path
) -> Generator[None, None, None]:
    run_host_setup(
        docker_client,
        task_directory,
        network_topology.CLUSTER.hosts,
        setup_agent_file_name="setup_agent.py",
    )
    time.sleep(1)
    start_bmp_client(
        docker_client,
        task_directory,
        hostname=MONITORING_HOST_NAME,
        bmp_client_file_name="bmp_client.py",
        log_file_name=CLIENT_LOG_FILE_NAME,
    )
    copy_file_to_container(
        docker_client,
        task_directory,
        hostname=MONITORING_HOST_NAME,
        filename="reader.py",
    )
    yield
    logging.info(
        "bmp_client log: %s",
        docker_client.execute_command_in_container(
            MONITORING_HOST_NAME, ["cat", CLIENT_LOG_FILE_NAME]
        ),
    )


def peer_down(client: DockerClient, peer_ip: IPv4Address, hostname: str) -> None:
    logging.info("shutdown peer %s at host %s", peer_ip, hostname)
    client.execute_command_in_container(
        hostname,
        [
            "vtysh",
            "-c",
            "config",
            "-c",
            "router bgp 65001",
            "-c",
            f"neighbor {peer_ip} shutdown",
        ],
    )
    time.sleep(1)


def peer_up(client: DockerClient, peer_ip: IPv4Address, hostname: str) -> None:
    logging.info("up peer %s at host %s", peer_ip, hostname)
    client.execute_command_in_container(
        hostname,
        [
            "vtysh",
            "-c",
            "config",
            "-c",
            "router bgp 65001",
            "-c",
            f"no neighbor {peer_ip} shutdown",
        ],
    )
    time.sleep(1)


def test_bmp(docker_client: DockerClient) -> None:
    output = docker_client.execute_command_in_container(
        MONITORING_HOST_NAME, ["python3", "/reader.py"]
    )
    assert output is not None and int(output) == 3


def test_bmp_peer_down(docker_client: DockerClient) -> None:
    peer_down(docker_client, IPv4Address("10.1.0.0"), "neighbour1")
    output = docker_client.execute_command_in_container(
        MONITORING_HOST_NAME, ["python3", "/reader.py"]
    )
    assert output is not None and int(output) == 2


def test_bmp_peer_up(docker_client: DockerClient) -> None:
    peer_down(docker_client, IPv4Address("10.1.0.0"), "neighbour1")
    output = docker_client.execute_command_in_container(
        MONITORING_HOST_NAME, ["python3", "/reader.py"]
    )
    assert output is not None and int(output) == 2

    peer_up(docker_client, IPv4Address("10.1.0.0"), "neighbour1")
    output = docker_client.execute_command_in_container(
        MONITORING_HOST_NAME, ["python3", "/reader.py"]
    )
    assert output is not None and int(output) == 3


def test_bmp_session_started(docker_client: DockerClient) -> None:
    output = docker_client.execute_command_in_container(
        SPEAKER_HOST, ["vtysh", "-c", "show bmp"]
    )
    assert output is not None and "1 connected clients" in output


def test_stats_server_listen_only_local(docker_client: DockerClient) -> None:
    output = docker_client.execute_command_in_container(
        SPEAKER_HOST, ["bash", "-c", f"! nc -zv -w 2 10.0.0.1 {str(SPEKER_PORT)}"]
    )
    # Stats server should listen on local host only.
    assert output == ""
