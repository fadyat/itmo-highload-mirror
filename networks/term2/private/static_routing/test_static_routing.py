#!/bin/env python3

import logging

import network_topology

from pathlib import Path

from test_infra.containerlab.pinger import Pinger
from test_infra.containerlab.host_setup import run_host_setup

# Configure logging
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)

def test_pairwise_ping(docker_client, task_directory):
    """Tests that ping from any host to any interface is successful."""

    run_host_setup(docker_client, task_directory, network_topology.CLUSTER.hosts)
    pinger = Pinger(docker_client, network_topology.CLUSTER.hosts)
    assert pinger.ping_pairwise()