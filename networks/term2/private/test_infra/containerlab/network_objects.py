from dataclasses import dataclass

from ipaddress import IPv4Interface, ip_network, ip_address


@dataclass
class Route:
    subnet: ip_network
    next_hop: ip_address


@dataclass
class Interface:
    name: str
    ip: IPv4Interface
    routes: tuple[Route] = tuple()


@dataclass
class Host:
    name: str
    interfaces: tuple[Interface]


@dataclass
class Cluster:
    hosts: tuple[Host]