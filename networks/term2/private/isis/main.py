import socket
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

def setup_host():
  wait_for_isisd_process()

  # Your solution should be here,
  print(f"Setting up host {socket.gethostname()}...") 

if __name__ == "__main__":
  setup_host()