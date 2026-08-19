#!/usr/bin/env python3

import socket
import sys

SOCKET_PATH = "/tmp/gamelauncher.sock"

message = " ".join(sys.argv[1:]) if len(sys.argv) > 1 else "Hello from Python!"

try:
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:
        sock.connect(SOCKET_PATH)
        sock.sendall(message.encode("utf-8"))
        print(f"Sent: {message}")

        # Uncomment if your server sends a response.
        response = sock.recv(4096)
        print("Received:", response.decode())

except FileNotFoundError:
    print(f"Socket '{SOCKET_PATH}' does not exist.")
except ConnectionRefusedError:
    print("Connection refused.")
except Exception as e:
    print(f"Error: {e}")
