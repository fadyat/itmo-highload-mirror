from collections.abc import Sequence
from ipaddress import IPv4Interface

from test_infra.containerlab.network_objects import Interface, Host, Cluster


CLUSTER = Cluster(
    hosts=[
        Host(
            name="PC1",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("192.168.11.4/24"),
                ),
            ],
        ),
        Host(
            name="PC2",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("192.168.22.4/24"),
                ),
            ],
        ),
        Host(
            name="PC3",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("192.168.33.4/24"),
                ),
            ],
        ),
        Host(
            name="R1",
            interfaces=[
                Interface(
                    name="lo",
                    ip=IPv4Interface("10.10.10.1/32"),
                ),
                Interface(
                    name="eth1",
                    ip=IPv4Interface("192.168.1.1/24"),
                ),
                Interface(
                    name="eth2",
                    ip=IPv4Interface("192.168.2.1/24"),
                ),
                Interface(
                    name="eth3",
                    ip=IPv4Interface("192.168.11.1/24"),
                ),
            ],
        ),
        Host(
            name="R2",
            interfaces=[
                Interface(name="lo", ip=IPv4Interface("10.10.10.2/32")),
                Interface(
                    name="eth1",
                    ip=IPv4Interface("192.168.1.2/24"),
                ),
                Interface(
                    name="eth2",
                    ip=IPv4Interface("192.168.3.2/24"),
                ),
                Interface(name="eth3", ip=IPv4Interface("192.168.22.2/24")),
            ],
        ),
        Host(
            name="R3",
            interfaces=[
                Interface(name="lo", ip=IPv4Interface("10.10.10.3/32")),
                Interface(
                    name="eth1",
                    ip=IPv4Interface("192.168.2.3/24"),
                ),
                Interface(
                    name="eth2",
                    ip=IPv4Interface("192.168.3.3/24"),
                ),
                Interface(name="eth3", ip=IPv4Interface("192.168.33.3/24")),
            ],
        ),
    ]
)
