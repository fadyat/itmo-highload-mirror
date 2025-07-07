from ipaddress import IPv4Interface

from test_infra.containerlab.network_objects import Interface, Host, Cluster


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
        ),
        Host(
            name="neighbour1",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.1.0.1/24"),
                ),
            ],
        ),
        Host(
            name="neighbour2",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.2.0.1/24"),
                ),
            ],
        ),
        Host(
            name="neighbour3",
            interfaces=[
                Interface(
                    name="eth1",
                    ip=IPv4Interface("10.3.0.1/24"),
                ),
            ],
        ),
    ]
)
