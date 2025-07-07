#!/usr/bin/env python3
import os
import shutil
import subprocess
import argparse
import logging

from pathlib import Path

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

def get_git_tracked_files(repo_path):
    """Get list of all tracked files in the git repository."""
    try:
        # Change to repository directory
        original_dir = os.getcwd()
        os.chdir(repo_path)
        
        # Get list of tracked files using git ls-files
        result = subprocess.run(['git', 'ls-files'], 
                              capture_output=True, 
                              text=True, 
                              check=True)
        
        # Change back to original directory
        os.chdir(original_dir)
        
        # Return list of files
        return result.stdout.splitlines()
    
    except subprocess.CalledProcessError as e:
        logger.error(f"Failed to get git tracked files: {e}")
        return []

def copy_to_public(private_repo_path, public_repo):
    """Copy all versioned files from private repo to public directory."""
    
    # Convert paths to Path objects
    private_path = Path(private_repo_path).resolve()
    public_path = Path(public_repo).resolve()
    
    # Check if private repo exists and is a git repository
    if not private_path.exists():
        raise ValueError(f"Private repository path does not exist: {private_path}")
    if not (private_path / '.git').exists():
        raise ValueError(f"Not a git repository: {private_path}")
    
    # Create public directory if it doesn't exist
    public_path.mkdir(parents=True, exist_ok=True)
    
    # Get list of tracked files
    tracked_files = get_git_tracked_files(private_path)
    
    # Copy each tracked file
    for file_name in tracked_files:
        source = private_path / file_name
        destination = public_path / file_name
        
        # Special handling for _template files
        if file_name.endswith('_template'):
            logger.info(f"Skipping {file_name} as it is a template")
            continue
        
        if (private_path / f'{file_name}_template').exists():
            logger.info(f"Copying {file_name}_template instead of {file_name}")
            source = private_path / f'{file_name}_template'

        # Create destination directory if it doesn't exist
        destination.parent.mkdir(parents=True, exist_ok=True)
        
        # Copy the file
        try:
            shutil.copy2(source, destination)
            logger.info(f"Copied: {file_name}")
        except Exception as e:
            logger.error(f"Error copying {file_name}: {e}")

def main():
    
    parser = argparse.ArgumentParser(
        description="Copy versioned files from a private git repository to a public directory"
    )
    parser.add_argument(
        "--private-repo",
        help="Path to the private git repository"
    )
    parser.add_argument(
        "--public-repo",
        help="Path to the public repository"
    )
    
    args = parser.parse_args()
    
    try:
        copy_to_public(args.private_repo, args.public_repo)
        logger.info("Copy completed successfully!")
    except Exception as e:
        logger.error(f"Error: {e}")
        exit(1)

if __name__ == "__main__":
    main()
