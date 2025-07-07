import socket
import os

config = {
    "PC1": {
        "interfaces": [
            ("eth1", "192.168.11.4/24"),
        ],
        "routes": [
            ("0.0.0.0/0", "192.168.11.1"),
        ]
    },
    "PC2": {
        "interfaces": [
            ("eth1", "192.168.22.4/24"),
        ],
        "routes": [
            ("0.0.0.0/0", "192.168.22.2"),
        ]
    },
    "PC3": {
        "interfaces": [
            ("eth1", "192.168.33.4/24"),
        ],
        "routes": [
            ("0.0.0.0/0", "192.168.33.3"),
        ],
    },
    "R1": {
        "interfaces": [
            ("eth1", "192.168.1.1/24"),
            ("eth2", "192.168.2.1/24"),
            ("eth3", "192.168.11.1/24"),
            ("lo", "10.10.10.1/32"),
        ],
        "routes": [
            ("192.168.22.0/24", "192.168.1.2"), # pc2 via r2
            ("192.168.33.0/24", "192.168.2.3"), # pc3 via r3
            ("192.168.3.0/24", "192.168.1.2"),
            ("10.10.10.2/32", "192.168.1.2"),
            ("10.10.10.3/32", "192.168.2.3")
        ],
    },
    "R2": {
        "interfaces": [
            ("eth1", "192.168.1.2/24"),
            ("eth2", "192.168.3.2/24"),
            ("eth3", "192.168.22.2/24"),
            ("lo", "10.10.10.2/32"),
        ],
        "routes": [
            ("192.168.11.0/24", "192.168.1.1"), # pc1 via r1
            ("192.168.33.0/24", "192.168.3.3"), # pc3 via r3
            ("192.168.2.0/24", "192.168.3.3"),
            ("10.10.10.1/32", "192.168.1.1"),
            ("10.10.10.3/32", "192.168.3.3")
        ]
    },
    "R3": {
        "interfaces": [
            ("eth1", "192.168.2.3/24"),
            ("eth2", "192.168.3.3/24"),
            ("eth3", "192.168.33.3/24"),
            ("lo", "10.10.10.3/32"),
        ],
        "routes": [
            ("192.168.22.0/24", "192.168.3.2"), # pc2 via r2
            ("192.168.11.0/24", "192.168.2.1"), # pc1 via r1
            ("192.168.1.0/24", "192.168.2.1"),
            ("10.10.10.1/32", "192.168.2.1"),
            ("10.10.10.2/32", "192.168.3.2")
        ]
    }
}

def setup_host():
    hostname = socket.gethostname()

    if hostname not in config:
        print(f"Host {hostname} not found in config")
        return

    host = config[hostname]
    print(f"Setting up host {hostname}")

    for interface, address in host["interfaces"]:
        print(f"Setting up interface {interface} with address {address}")
        os.system(f"ip address add {address} dev {interface}")

    for route, gateway in host["routes"]:
        print(f"Adding route {route} via {gateway}")
        os.system(f"ip route add {route} via {gateway}")

if __name__ == "__main__":
    setup_host()
