import concurrent.futures
import concurrent
import logging

from dataclasses import dataclass

from test_infra.containerlab.docker_client import DockerClient
from test_infra.containerlab.network_objects import Host, Interface

logger = logging.getLogger(__name__)

@dataclass(frozen=True)
class PingResult:
    src: Host
    dst_host: Host
    dst_interface: Interface
    ping_result_future: concurrent.futures.Future


class Pinger:
    """Sends pings between containers"""

    THREADPOOL_SIZE = 100

    def __init__(self, client: DockerClient, hosts: list[Host]):
        self.hosts: list[Host] = hosts
        self.docker_client: DockerClient = client
        self.executor = concurrent.futures.ThreadPoolExecutor(
            max_workers=self.THREADPOOL_SIZE
        )

    def ping_pairwise(self):
        """Sends ping to from every container to every container"""
        results: list[PingResult] = []
        for src_host in self.hosts:
            for dst_host in self.hosts:
                for dst_interface in dst_host.interfaces:
                    logging.debug(
                        "Do async pinging from host %s to ip %s on interface %s of %s",
                        src_host.name,
                        dst_interface.ip.ip,
                        dst_interface.name,
                        dst_host.name,
                    )
                    future = self.executor.submit(
                        self.docker_client.execute_command_in_container,
                        src_host.name,
                        ["ping", str(dst_interface.ip.ip), "-c", "3", "-W", "1", "-t", "10"],
                    )
                    results.append(
                        PingResult(
                            src=src_host,
                            dst_host=dst_host,
                            dst_interface=dst_interface,
                            ping_result_future=future,
                        )
                    )

        for ping_result in results:
            try:
                ret = ping_result.ping_result_future.result(timeout=10)
            except Exception as e:
                logger.error(
                    "Ping from host %s to interface %s on host %s failed. Error: %s",
                    ping_result.src.name,
                    ping_result.dst_interface.name,
                    ping_result.dst_host.name,
                    e,
                )
                return False

        logger.info("All pings succeeded.")
        return True
