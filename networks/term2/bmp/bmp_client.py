import enum
import logging
import socket
import struct
import threading
from argparse import ArgumentParser
from ipaddress import IPv4Address

# Configure global logging
logger = logging.getLogger(__name__)


class MessageType(enum.Enum):
    ROUTE_MONITORING = 0
    STATISTICS_REPORT = 1
    PEER_DOWN = 2
    PEER_UP = 3
    INITIATION = 4
    TERMINATION = 5
    ROUTE_MIRRORING = 6


class CommonHeader:
    SIZE = 6

    def __init__(self, raw: bytes):
        self.version = raw[0]
        self.message_length = struct.unpack('!I', raw[1:5])[0]
        self.message_type = MessageType(raw[5])


class PeerType(enum.Enum):
    GLOBAL_INSTANCE = 0
    RD_INSTANCE = 1
    LOCAL_INSTANCE = 2


class PeerFlags:
    def __init__(self, flags_byte: int):
        self.V = bool(flags_byte & 0b10000000)  # IPv6 flag
        self.L = bool(flags_byte & 0b01000000)  # Post-policy
        self.A = bool(flags_byte & 0b00100000)  # 2-byte ASN


class PerPeerHeader:
    SIZE = 42

    def __init__(self, raw: bytes):
        # self.peer_type = PeerType(raw[0])
        # self.peer_flags = PeerFlags(raw[1])
        # self.peer_distinguisher = struct.unpack('!Q', raw[2:10])[0]
        #
        # if self.peer_flags.V:
        #     self.peer_address = IPv6Address(raw[10:26])
        # else:
        #     self.peer_address = IPv4Address(raw[22:26])
        #
        # if self.peer_flags.A:
        #     self.peer_as = struct.unpack('!H', raw[26:28])[0]
        # else:
        #     self.peer_as = struct.unpack('!I', raw[26:30])[0]

        self.bgp_id = IPv4Address(raw[30:34])
        # self.timestamp = struct.unpack('!I', raw[34:38])[0]
        # self.timestamp_micro = struct.unpack('!I', raw[38:42])[0]


class PeerStats:
    def __init__(self) -> None:
        self.lock = threading.Lock()
        self.peer_count = 0

    def peer_up(self) -> None:
        with self.lock:
            self.peer_count += 1

    def peer_down(self) -> None:
        with self.lock:
            self.peer_count -= 1

    def established_count(self) -> int:
        with self.lock:
            return self.peer_count


class BMPClient:
    def __init__(self, host: IPv4Address, port: int, peer_stats: PeerStats) -> None:
        self.host: IPv4Address = host
        self.port: int = port
        self.peer_stats: PeerStats = peer_stats
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def start(self) -> None:
        try:
            self.sock.connect((str(self.host), self.port))
            logger.info("connected to BMP server at %s:%d", self.host, self.port)

            while True:
                common_header = CommonHeader(self._recv(CommonHeader.SIZE))
                if common_header.message_type not in (MessageType.PEER_UP, MessageType.PEER_DOWN):
                    self._recv(common_header.message_length - CommonHeader.SIZE)
                    continue

                per_peer_header = PerPeerHeader(self._recv(PerPeerHeader.SIZE))
                if per_peer_header.bgp_id != self.host:
                    match common_header.message_type:
                        case MessageType.PEER_UP:
                            self.peer_stats.peer_up()
                        case MessageType.PEER_DOWN:
                            self.peer_stats.peer_down()

                self._recv(common_header.message_length - CommonHeader.SIZE - PerPeerHeader.SIZE)

        except Exception as e:
            logger.error("BMP client error: %s", e)
        finally:
            self.sock.close()
            logger.info("BMP client disconnected")

    def _recv(self, n: int) -> bytes | None:
        data = bytearray()
        while len(data) < n:
            try:
                packet = self.sock.recv(n - len(data))
                if not packet:
                    return None
                data.extend(packet)
            except (ConnectionResetError, BrokenPipeError, OSError):
                return None

        return bytes(data)


class StatsServer:
    def __init__(self, host: IPv4Address, port: int, peer_stats: PeerStats) -> None:
        self.host: IPv4Address = host
        self.port: int = port
        self.peer_stats = peer_stats
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    def start(self) -> None:
        try:
            self.sock.bind((str(self.host), self.port))
            self.sock.listen(5)
            logger.info("stats server listening on %s:%d", self.host, self.port)

            while True:
                conn, addr = self.sock.accept()
                threading.Thread(target=self._handle_client, args=(conn,), daemon=True).start()
        except Exception as e:
            logger.error("stats server error: %s", e)
        finally:
            self.sock.close()
            logger.info("stats server stopped")

    def _handle_client(self, conn: socket.socket) -> None:
        try:
            count = self.peer_stats.established_count()
            data = struct.pack('!I', count)
            conn.sendall(data)
        except Exception as e:
            logger.error("error handling stats client: %s", e)
        finally:
            conn.close()


def get_arguments():
    parser = ArgumentParser()
    parser.add_argument(
        "--bmp-ip-addr", type=IPv4Address, help="IP address of monitored router"
    )
    parser.add_argument(
        "--bmp-port", type=int, help="TCP port of BMP server on monitored router"
    )
    parser.add_argument(
        "--stats-server-ip", type=IPv4Address, help="IP address to bind stats server"
    )
    parser.add_argument(
        "--stats-server-port", type=int, help="TCP port stats server should listen on"
    )
    parser.add_argument("--log-file", help="File path for logging")

    args = parser.parse_args()

    # Configure logging
    logging.basicConfig(
        filename=args.log_file,
        filemode="a",
        level=logging.INFO,
        format="%(asctime)s - %(levelname)s - %(message)s",
    )

    return args


def main():
    args = get_arguments()
    peer_stats = PeerStats()

    bmp_client = BMPClient(
        host=IPv4Address(args.bmp_ip_addr), port=args.bmp_port, peer_stats=peer_stats
    )
    stats_server = StatsServer(
        host=IPv4Address(args.stats_server_ip),
        port=args.stats_server_port,
        peer_stats=peer_stats,
    )

    threading.Thread(target=bmp_client.start, daemon=True).start()
    threading.Thread(target=stats_server.start, daemon=True).start()

    threading.Event().wait()


if __name__ == "__main__":
    main()
