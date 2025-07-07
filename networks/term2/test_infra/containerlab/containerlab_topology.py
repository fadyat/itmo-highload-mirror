#!/bin/env python3

import logging
import yaml
import subprocess

from pathlib import Path


logger = logging.getLogger(__name__)


class ContainerlabTopology:
    """Class to handle container lab start/stop.

    This class has side effect of creating new topology file in workdir, to give
    the topology unique name
    containerlab deploy has --name argument, while destroy doesn't. As result we have to pass
    name of topology through topology file.

    """

    def __init__(
        self,
        workdir: Path,
        task_directory: Path,
        topology_file_name: str,
        topology_name_prefix: str,
    ) -> None:
        self.workdir: Path = workdir
        self.task_directory: Path = task_directory
        self.topology_file_name: str = topology_file_name
        self.topology_name_prefix: str = topology_name_prefix
        self.new_topology_path: Path = self.workdir / self.topology_file_name
        self.topology_file_set_up: bool = False
        self.new_topology_name: str | None = None

    def setup_topology_file(self) -> None:
        """Creates new topology file in workdir with overridden name.

        Also sets self.new_topology_name to actual value.
        """
        with open(self.task_directory / self.topology_file_name, "r") as topology:
            parsed = yaml.safe_load(topology)

            if "name" not in parsed:
                raise ValueError("Topology doesn't have name field.")

            self.new_topology_name = parsed["name"] + self.topology_name_prefix
            parsed["name"] = self.new_topology_name
            with open(self.new_topology_path, "w") as out:
                yaml.safe_dump(parsed, out)

        self.topology_file_set_up = True

    def get_new_topology_name(self) -> str | None:
        """Returns new topology name or None if new topology file is not set up."""
        return self.new_topology_name

    def start(self) -> bool:
        """Starts new containerlab topology requires topology file to be set up already.

        Returns: True if topology started successfully False otherwise.
        """

        if not self.topology_file_set_up:
            raise RuntimeError(
                "ContainerlabTopology.start called, before ContainerlabTopology.setup_topology_file."
            )

        return_code = subprocess.call(
            ["containerlab", "deploy", "-t", self.new_topology_path, "--reconfigure"],
            env={"CLAB_LABDIR_BASE": self.workdir},
        )

        return not return_code

    def cleanup(self) -> None:
        """Destroys running topology requires topology file to be set up already."""

        if not self.topology_file_set_up:
            raise RuntimeError(
                "ContainerlabTopology.cleanup called, before ContainerlabTopology.setup_topology_file."
            )

        subprocess.check_call(["containerlab", "destroy", "-t", self.new_topology_path])
