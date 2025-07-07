import socket
import subprocess
import logging
import copy
import time

from dataclasses import dataclass
from ipaddress import IPv4Interface, IPv4Network, IPv4Address


@dataclass(frozen=True)
class BgpNeighbour:
    ip: IPv4Address


@dataclass(frozen=True)
class Bmp:
    port: int
    listen_ip: IPv4Address


@dataclass(frozen=True)
class Bgp:
    asn: int
    router_id: IPv4Address
    neighbours: list[BgpNeighbour]
    bmp: None | Bmp = None


@dataclass(frozen=True)
class Interface:
    name: str
    ip: IPv4Interface
    bgp_peer: bool = True


@dataclass(frozen=True)
class Host:
    name: str
    interfaces: list[Interface]
    bgp: Bgp | None = None


@dataclass(frozen=True)
class Cluster:
    hosts: list[Host]


CLUSTER = Cluster(
    hosts=[
        Host(
            name="monitoring-client",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.0.0.1/24"),
                ),
            ],
        ),
        Host(
            name="speaker",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.1.0.0/24"),
                ),
                Interface(
                    name="eth2",
                    ip=IPv4Interface("10.2.0.0/24"),
                ),
                Interface(
                    name="eth3",
                    ip=IPv4Interface("10.3.0.0/24"),
                ),
                Interface(
                    name="eth4",
                    ip=IPv4Interface("10.0.0.0/24"),
                ),
            ],
            bgp=Bgp(
                asn=65000,
                router_id=IPv4Address("10.0.0.0"),
                neighbours=[
                    BgpNeighbour(
                        ip=IPv4Address("10.1.0.1"),
                    ),
                    BgpNeighbour(
                        ip=IPv4Address("10.2.0.1"),
                    ),
                    BgpNeighbour(
                        ip=IPv4Address("10.3.0.1"),
                    ),
                ],
                bmp=Bmp(
                    port=11019,
                    listen_ip=IPv4Address("10.0.0.0"),
                ),
            ),
        ),
        Host(
            name="neighbour1",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.1.0.1/24"),
                ),
            ],
            bgp=Bgp(
                asn=65001,
                router_id=IPv4Address("10.1.0.1"),
                neighbours=[
                    BgpNeighbour(
                        ip=IPv4Address("10.1.0.0"),
                    ),
                ],
            ),
        ),
        Host(
            name="neighbour2",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.2.0.1/24"),
                ),
            ],
            bgp=Bgp(
                asn=65002,
                router_id=IPv4Address("10.2.0.1"),
                neighbours=[
                    BgpNeighbour(
                        ip=IPv4Address("10.2.0.0"),
                    ),
                ],
            ),
        ),
        Host(
            name="neighbour3",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.3.0.1/24"),
                ),
            ],
            bgp=Bgp(
                asn=65003,
                router_id=IPv4Address("10.3.0.1"),
                neighbours=[
                    BgpNeighbour(
                        ip=IPv4Address("10.3.0.0"),
                    ),
                ],
            ),
        ),
    ]
)


class Client:
    """Client to vtysh shell running in docker container."""

    def __init__(self) -> None:
        pass

    def exec_command(self, command: str | list[str]) -> tuple[int, str, str]:
        if isinstance(command, str):
            command = [command]

        args = ["vtysh"]
        for comm in command:
            args.append("-c")
            args.append(comm)
        result = subprocess.run(args, capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr


class Configurator:
    """Utily class to generate vtysh commands for machine configuration."""

    def __init__(self) -> None:
        self.commands: list[str] = []

    def __add_commands(self, commands: list[str]) -> None:
        self.commands.extend(commands)

    def set_interface_ip_addr(self, interface_name: str, ip: IPv4Interface) -> None:
        self.__add_commands(
            ["configure", f"int {interface_name}", f"ip addr {str(ip)}", "exit", "exit"]
        )

    def enable_bgp(
        self,
        bgp_conf: Bgp,
    ) -> None:
        neighbours = [f"neighbor {n.ip} peer-group ALL" for n in bgp_conf.neighbours]
        bmp = list()
        if bgp_conf.bmp is not None:
            bmp = [
                "bmp target all",
                f"bmp listener {bgp_conf.bmp.listen_ip} port {bgp_conf.bmp.port}",
                "exit",
            ]

        self.__add_commands(
            [
                "configure",
                f"router bgp {bgp_conf.asn}",
                f"bgp router-id {bgp_conf.router_id}",
                "neighbor ALL peer-group",
                "neighbor ALL remote-as external",
            ]
            + neighbours
            + bmp
            + ["address-family ipv4 unicast"]
            + ["exit"]
            + ["exit"]
        )

    def do_write(self) -> None:
        self.__add_commands(["do write"])

    def get_commands(self) -> list[str]:
        # Make copy to avoid modification of self.commands list
        # by caller.
        return copy.deepcopy(self.commands)


def wait_for_bgpd_process(seconds_to_wait: int = 10) -> None:
    for _ in range(seconds_to_wait):
        try:
            subprocess.check_output(["pgrep", "bgpd"])
            return
        except subprocess.CalledProcessError:
            time.sleep(1)

    raise RuntimeError("bgpd process did not start within timeout")


def setup_host() -> None:
    wait_for_bgpd_process()

    hostname = socket.gethostname()
    configurator = Configurator()

    host = next(host for host in CLUSTER.hosts if host.name == hostname)

    for interface in host.interfaces:
        configurator.set_interface_ip_addr(interface.name, interface.ip)

    bgp = host.bgp
    if bgp is not None:
        configurator.enable_bgp(bgp)

    configurator.do_write()

    commands = configurator.get_commands()
    print(*commands, sep="\n")
    returncode, output, stderr = Client().exec_command(commands)
    print(f"{returncode=}, {output=}, {stderr=}")

    assert returncode == 0, "Failed to configure host"


if __name__ == "__main__":
    setup_host()
