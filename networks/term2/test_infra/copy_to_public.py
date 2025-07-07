#!/usr/bin/env python3
import os
import shutil
import subprocess
import argparse
import logging
from pathlib import Path

# Configure logging
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)


def get_git_tracked_files(repo_path: Path) -> list[Path]:
    """Get list of all tracked files in the git repository."""
    try:
        # Change to repository directory
        original_dir: Path = Path.cwd()
        os.chdir(repo_path)

        # Get list of tracked files using git ls-files
        result: subprocess.CompletedProcess[str] = subprocess.run(
            ["git", "ls-files"], capture_output=True, text=True, check=True
        )

        # Change back to original directory
        os.chdir(original_dir)

        # Return list of Path objects
        return [Path(line) for line in result.stdout.splitlines()]

    except subprocess.CalledProcessError as e:
        logger.error(f"Failed to get git tracked files: {e}")
        return []


def copy_to_public(private_repo_path: Path, public_repo: Path) -> None:
    """Copy all versioned files from private repo to public directory."""

    # Convert paths to Path objects
    private_path: Path = private_repo_path.resolve()
    public_path: Path = public_repo.resolve()

    # Check if private repo exists and is a git repository
    if not private_path.exists():
        raise ValueError(f"Private repository path does not exist: {private_path}")
    if not (private_path / ".git").exists():
        raise ValueError(f"Not a git repository: {private_path}")

    # Create public directory if it doesn't exist
    public_path.mkdir(parents=True, exist_ok=True)

    # Get list of tracked files
    tracked_files: list[Path] = get_git_tracked_files(private_path)

    # Copy each tracked file
    for file_path in tracked_files:
        source: Path = private_path / file_path
        destination: Path = public_path / file_path

        # Special handling for _template files
        if file_path.name.endswith("_template"):
            logger.info(f"Skipping {file_path} as it is a template")
            continue

        template_path = private_path / f"{file_path}_template"
        if template_path.exists():
            logger.info(f"Copying {template_path} instead of {file_path}")
            source = template_path

        # Create destination directory if it doesn't exist
        destination.parent.mkdir(parents=True, exist_ok=True)

        # Copy the file
        try:
            shutil.copy2(source, destination)
            logger.info(f"Copied: {file_path}")
        except Exception as e:
            logger.error(f"Error copying {file_path}: {e}")


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Copy versioned files from a private git repository to a public directory"
    )
    parser.add_argument(
        "--private-repo", type=Path, help="Path to the private git repository"
    )
    parser.add_argument(
        "--public-repo", type=Path, help="Path to the public repository"
    )

    args: argparse.Namespace = parser.parse_args()

    try:
        copy_to_public(args.private_repo, args.public_repo)
        logger.info("Copy completed successfully!")
    except Exception as e:
        logger.error(f"Error: {e}")
        exit(1)


if __name__ == "__main__":
    main()
