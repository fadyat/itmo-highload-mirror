import socket
import subprocess
import time


# Start of isisd can take a few seconds after container start.
# Attempt at host configuration without running isisd will lead to
# an obscure vtysh failure.
def wait_for_isisd_process(seconds_to_wait: int = 10) -> None:
    for _ in range(seconds_to_wait):
        try:
            subprocess.check_output(["pgrep", "isisd"])
            return
        except subprocess.CalledProcessError:
            time.sleep(1)

    raise RuntimeError("isisd process did not start within timeout")


def cmd(command: str, _input: bytes | None = None, ):
    subprocess.run(command, input=_input, shell=True, check=True, capture_output=True)


devices = {
    "PC1": {
        "net": "49.0001.0000.0000.0001.00",
        "ifaces": ("eth1",),
        "ip": ("ip addr add 192.168.11.4/24 dev eth1",)
    },
    "PC2": {
        "net": "49.0001.2000.0000.2002.00",
        "ifaces": ("eth1",),
        "ip": ("ip addr add 192.168.22.4/24 dev eth1",)
    },
    "PC3": {
        "net": "49.0001.3000.0000.3003.00",
        "ifaces": ("eth1",),
        "ip": ("ip addr add 192.168.33.4/24 dev eth1",)
    },
    "R1": {
        "net": "49.0001.1000.0000.1001.00",
        "ifaces": ("lo", "eth1", "eth2", "eth3"),
        "ip": (
            "ip addr add 10.10.10.1/32 dev lo",
            "ip addr add 192.168.1.1/24 dev eth1",
            "ip addr add 192.168.2.1/24 dev eth2",
            "ip addr add 192.168.11.1/24 dev eth3",
        )
    },
    "R2": {
        "net": "49.0001.2000.0000.2000.00",
        "ifaces": ("lo", "eth1", "eth2", "eth3"),
        "ip": (
            "ip addr add 10.10.10.2/32 dev lo",
            "ip addr add 192.168.1.2/24 dev eth1",
            "ip addr add 192.168.3.2/24 dev eth2",
            "ip addr add 192.168.22.2/24 dev eth3",
        )
    },
    "R3": {
        "net": "49.0001.3000.0000.3000.00",
        "ifaces": ("lo", "eth1", "eth2", "eth3"),
        "ip": (
            "ip addr add 10.10.10.3/32 dev lo",
            "ip addr add 192.168.2.3/24 dev eth1",
            "ip addr add 192.168.3.3/24 dev eth2",
            "ip addr add 192.168.33.3/24 dev eth3",
        ),
    }
}


def setup_network_interfaces(host: str):
    if host not in devices:
        raise ValueError(f"Unknown host: {host}")

    for c in devices[host]["ip"]:
        cmd(c)


def setup_vtysh(host: str):
    if host not in devices:
        raise ValueError(f"Unknown host: {host}")

    c = [
        "configure terminal",
        "router isis 1",
        "is-type level-2-only",
        f"net {devices[host]['net']}",
        "spf-interval 2",
        "lsp-refresh-interval 2",
        "lsp-gen-interval 1",
        "exit"
    ]

    for iface in devices[host]["ifaces"]:
        c.append(f"interface {iface}")
        c.append("ip router isis 1")
        if iface != "lo":
            c.append("isis circuit-type level-2-only")
            c.append("isis network point-to-point")
        else:
            c.append("isis passive")

        c.append("exit")

    c.extend(["exit", "write"])

    _input = "\n".join(c).encode()
    cmd("vtysh", _input=_input)


def setup_host():
    wait_for_isisd_process()

    hostname = socket.gethostname()
    setup_network_interfaces(hostname)
    setup_vtysh(hostname)


if __name__ == "__main__":
    setup_host()
