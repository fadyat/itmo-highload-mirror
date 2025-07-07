import pathlib

def pytest_configure(config):
    config.pluginmanager.unregister(name="cacheprovider")

def pytest_addoption(parser):
    parser.addoption(
        "--workdir", type=pathlib.Path, help="Directory to store temporary data."
    )
    parser.addoption(
        "--task_directory", type=pathlib.Path, help="Path to the task directory"
    )
    parser.addoption(
        "--topology_file_name",
        default=[],
        action='append',
        help="Name of containerlab topology file.",
    )
    parser.addoption(
        "--course_unique_commit_id",
        default="",
        help="Unique string to avoid collisions in container names.",
    )

# Run all tests for all topology files listed in commandline arguments.
def pytest_generate_tests(metafunc):
    if "topology_file_name" in metafunc.fixturenames:
        metafunc.parametrize("topology_file_name", metafunc.config.getoption("--topology_file_name"))
