import dataclasses
import time
import subprocess
import socket
import typing


@dataclasses.dataclass
class D:
    asn: int
    ifaces: typing.Dict[str, str]


def cmd(command: str, inp: typing.Optional[bytes] = None):
    subprocess.run(command, input=inp, shell=True, check=True, capture_output=True)


# Start of bgpd can take a few seconds after container start.
# Attempt at host configuration without running bgpd will lead to
# an obscure vtysh failure.
def wait_for_bgpd_process(seconds_to_wait: int = 10) -> None:
    for _ in range(seconds_to_wait):
        try:
            subprocess.check_output(["pgrep", "bgpd"])
            return
        except subprocess.CalledProcessError:
            time.sleep(1)

    raise RuntimeError("bgpd process did not start within timeout")


devices = {
    "pc1": D(65100, {
        "lo": "10.0.254.7/32",
        "eth1": "172.16.11.1/30",
    }),
    "pc2": D(65101, {
        "lo": "10.0.254.8/32",
        "eth1": "172.16.22.1/30",
    }),
    "leaf1": D(65001, {
        "lo": "10.0.254.5/32",
        "eth1": "172.16.112.2/30",
        "eth2": "172.16.111.2/30",
        "eth3": "172.16.11.2/30",
        "eth4": "172.16.113.2/30",
        "eth5": "172.16.114.2/30",
    }),
    "leaf2": D(65002, {
        "lo": "10.0.254.6/32",
        "eth1": "172.16.221.2/30",
        "eth2": "172.16.222.2/30",
        "eth3": "172.16.22.2/30",
        "eth4": "172.16.223.2/30",
        "eth5": "172.16.224.2/30",
    }),
    "spine1": D(65000, {
        "lo": "10.0.254.1/32",
        "eth1": "172.16.221.1/30",
        "eth2": "172.16.111.1/30",
    }),
    "spine2": D(65000, {
        "lo": "10.0.254.2/32",
        "eth1": "172.16.112.1/30",
        "eth2": "172.16.222.1/30",
    }),
    "spine3": D(65200, {
        "lo": "10.0.254.3/32",
        "eth1": "172.16.223.1/30",
        "eth2": "172.16.113.1/30",
    }),
    "spine4": D(65200, {
        "lo": "10.0.254.4/32",
        "eth1": "172.16.114.1/30",
        "eth2": "172.16.224.1/30",
    })
}

routing_commands = {
    "common": lambda: (
        "ip prefix-list DC_LOCAL_SUBNET seq 5 permit 172.16.0.0/16 le 32",
        "ip prefix-list DC_LOCAL_SUBNET seq 10 permit 10.0.254.0/24 le 32",
        "route-map PERMIT_EBGP permit 10",
        "route-map ACCEPT_DC_LOCAL permit 10",
        "match ip address prefix-list DC_LOCAL_SUBNET",
    ),
    "pc": lambda asn,
                 router_ip,
                 leaf_iface="eth1":
    (
        f"router bgp {asn}",
        f"bgp router-id {router_ip}",
        "neighbor eth1 interface remote-as external",
        "address-family ipv4 unicast",
        "redistribute connected route-map ACCEPT_DC_LOCAL",
        f"neighbor {leaf_iface} route-map PERMIT_EBGP in",
        f"neighbor {leaf_iface} route-map PERMIT_EBGP out"
    ),
    "leaf": lambda asn,
                   router_ip,
                   pc_iface="eth3",
                   peer_group_ifaces=("eth1", "eth2", "eth4", "eth5"):
    (
        f"router bgp {asn}",
        f"bgp router-id {router_ip}",
        "bgp bestpath as-path multipath-relax",
        "timers bgp 3 9",
        "neighbor FABRIC peer-group",
        "neighbor FABRIC remote-as external",
        "neighbor FABRIC advertisement-interval 0",
        "neighbor FABRIC timers connect 5",
        *[
            f"neighbor {iface} interface peer-group FABRIC"
            for iface in peer_group_ifaces
        ],
        f"neighbor {pc_iface} interface remote-as external",
        "address-family ipv4 unicast",
        "redistribute connected route-map ACCEPT_DC_LOCAL",
        "neighbor FABRIC route-map PERMIT_EBGP in",
        "neighbor FABRIC route-map PERMIT_EBGP out",
        f"neighbor {pc_iface} route-map PERMIT_EBGP in",
        f"neighbor {pc_iface} route-map PERMIT_EBGP out",
    ),
    "spine": lambda asn,
                    router_ip,
                    peer_group_ifaces=("eth1", "eth2"):
    (
        f"router bgp {asn}",
        f"bgp router-id {router_ip}",
        "bgp bestpath as-path multipath-relax",
        "timers bgp 3 9",
        "neighbor FABRIC peer-group",
        "neighbor FABRIC remote-as external",
        "neighbor FABRIC advertisement-interval 0",
        "neighbor FABRIC timers connect 5",
        *[
            f"neighbor {iface} interface peer-group FABRIC"
            for iface in peer_group_ifaces
        ],
        "address-family ipv4 unicast",
        "redistribute connected route-map ACCEPT_DC_LOCAL",
        "neighbor FABRIC route-map PERMIT_EBGP in",
        "neighbor FABRIC route-map PERMIT_EBGP out",
    )
}


def routing_commands_by_hostname(hostname: str, d: D) -> typing.Tuple[str, ...]:
    common_commands = routing_commands["common"]()

    for prefix in ("pc", "leaf", "spine"):
        if hostname.startswith(prefix):
            lo = d.ifaces["lo"].split("/")[0]
            return common_commands + routing_commands[prefix](d.asn, lo)

    return common_commands


def setup_network_interfaces(hostname: str):
    d = devices[hostname]
    for eth, ip in d.ifaces.items():
        cmd(f"ip addr add {ip} dev {eth}")


def setup_vtysh(hostname: str):
    commands = routing_commands_by_hostname(hostname, devices[hostname])
    cmd("vtysh -c conf " + " ".join(f'-c "{c}"' for c in commands))


def setup_host():
    wait_for_bgpd_process()

    hostname = socket.gethostname().lower()
    setup_network_interfaces(hostname)
    setup_vtysh(hostname)


if __name__ == "__main__":
    setup_host()
