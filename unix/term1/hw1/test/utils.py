from hypothesis import settings
from pathlib import Path
import subprocess


TESTS_DIR = Path(__file__).resolve().parent
TMP_DIR = TESTS_DIR.joinpath('tmp')

ROOT_DIR = TESTS_DIR.parent
SRC_DIR = ROOT_DIR.joinpath('src')


def run_command(cmd, inp='', env=None):
    p = subprocess.run(cmd, input=inp, capture_output=True, text=True, env=env, timeout=2)
    return p.returncode, p.stdout, p.stderr


TMP_DIR.mkdir(mode=0o755, exist_ok=True)

settings.register_profile("test", deadline=500)
settings.load_profile("test")
