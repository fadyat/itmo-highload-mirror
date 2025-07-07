#!/bin/env python3

import logging

import static_routing_backup_link.network_topology as network_topology

from pathlib import Path

from test_infra.containerlab.pinger import Pinger
from test_infra.containerlab.host_setup import run_host_setup
from test_infra.containerlab.docker_client import DockerClient

logger = logging.getLogger(__name__)


def test_pairwise_ping(docker_client: DockerClient, task_directory: Path) -> None:
    """Tests that ping from any host to any interface is successful."""

    run_host_setup(docker_client, task_directory, network_topology.CLUSTER.hosts)

    logger.info("run pairwise ping")
    pinger = Pinger(docker_client, network_topology.CLUSTER.hosts)
    assert pinger.ping_pairwise()


def test_pairwise_ping_cut_r1_r2(
    docker_client: DockerClient, task_directory: Path
) -> None:
    run_host_setup(docker_client, task_directory, network_topology.CLUSTER.hosts)

    logger.info("cut r1-r2 link")
    docker_client.execute_command_in_container(
        "R1", ["ip", "link", "set", "eth1", "down"]
    )
    docker_client.execute_command_in_container(
        "R2", ["ip", "link", "set", "eth1", "down"]
    )

    logger.info("run pairwise ping")
    pinger = Pinger(docker_client, network_topology.CLUSTER.hosts)
    assert pinger.ping_pairwise()


def test_pairwise_ping_cut_r1_r3(
    docker_client: DockerClient, task_directory: Path
) -> None:
    run_host_setup(docker_client, task_directory, network_topology.CLUSTER.hosts)

    logger.info("cut r1-r3 link")
    docker_client.execute_command_in_container(
        "R1", ["ip", "link", "set", "eth2", "down"]
    )
    docker_client.execute_command_in_container(
        "R3", ["ip", "link", "set", "eth1", "down"]
    )

    logger.info("run pairwise ping")
    pinger = Pinger(docker_client, network_topology.CLUSTER.hosts)
    assert pinger.ping_pairwise()


def test_pairwise_ping_cut_r3_r2(
    docker_client: DockerClient, task_directory: Path
) -> None:
    run_host_setup(docker_client, task_directory, network_topology.CLUSTER.hosts)

    logger.info("cut r2-r3 link")
    docker_client.execute_command_in_container(
        "R3", ["ip", "link", "set", "eth2", "down"]
    )
    docker_client.execute_command_in_container(
        "R2", ["ip", "link", "set", "eth2", "down"]
    )

    logger.info("run pairwise ping")
    pinger = Pinger(docker_client, network_topology.CLUSTER.hosts)
    assert pinger.ping_pairwise()
