import socket
import struct

HOST = "127.0.0.1"  # Change as needed
PORT = 17957  # Change as needed


def main() -> None:
    with socket.create_connection((HOST, PORT)) as conn:
        data = conn.recv(4)
        if len(data) < 4:
            print(-1)
        else:
            value = struct.unpack("!I", data)[0]
            print(value)


if __name__ == "__main__":
    main()
