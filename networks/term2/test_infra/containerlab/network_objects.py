from dataclasses import dataclass, field

from ipaddress import IPv4Interface, IPv4Network, IPv4Address


@dataclass
class Route:
    subnet: IPv4Network
    next_hop: IPv4Address


@dataclass
class Interface:
    name: str
    ip: IPv4Interface
    routes: list[Route] = field(default_factory=list)


@dataclass
class Host:
    name: str
    interfaces: list[Interface] = field(default_factory=list)


@dataclass
class Cluster:
    hosts: list[Host] = field(default_factory=list)
