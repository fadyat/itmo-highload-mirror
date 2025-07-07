#!/bin/env python3

import logging
import time

import bgp.network_topology as network_topology

from pathlib import Path

from test_infra.containerlab.docker_client import DockerClient
from test_infra.containerlab.pinger import Pinger
from test_infra.containerlab.host_setup import run_host_setup

logger = logging.getLogger(__name__)


def test_pairwise_ping(docker_client: DockerClient, task_directory: Path) -> None:
    """Tests that ping from any host to any interface is successful."""
    run_host_setup(docker_client, task_directory, network_topology.CLUSTER.hosts)
    logger.info("waiting for routes to converge")
    time.sleep(5)

    logger.info("run pairwise ping")
    pinger = Pinger(docker_client, network_topology.CLUSTER.hosts)
    assert pinger.ping_pairwise()
