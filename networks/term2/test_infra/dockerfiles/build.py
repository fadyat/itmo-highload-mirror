#!/usr/bin/env python3

import subprocess
import sys
import argparse
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)

# List of Docker images to build
DOCKER_IMAGES: list[str] = ["testenv", "containerlab", "frrenv"]


def run_command(cmd: list[str]) -> None:
    """Run a shell command and print it for debugging."""
    logger.info(f"Running: {' '.join(cmd)}")
    try:
        subprocess.run(cmd, check=True)
        logger.debug(f"Command completed successfully: {' '.join(cmd)}")
    except subprocess.CalledProcessError as e:
        logger.error(f"Command failed with exit code {e.returncode}: {' '.join(cmd)}")
        raise


def build_and_push_images(registry: str, tag: str, images: list[str]) -> None:
    """Build and optionally push Docker images."""
    # Format registry and tag
    formatted_registry: str = f"{registry}/" if registry else ""
    formatted_tag: str = f":{tag}" if tag else ""

    logger.info("Starting Docker image build process")
    logger.debug(f"Using registry: {formatted_registry}, tag: {formatted_tag}")

    # Validate image names
    invalid_images = [img for img in images if img not in DOCKER_IMAGES]
    if invalid_images:
        logger.error(
            f"Invalid image names: {', '.join(invalid_images)}. Valid images are: {', '.join(DOCKER_IMAGES)}"
        )
        sys.exit(1)

    logger.info(f"Will build {len(images)} images: {', '.join(images)}")

    # Build images
    for image in images:
        build_image_name: str = f"{formatted_registry}{image}{formatted_tag}"
        logger.info(f"Building image: {build_image_name}")
        logger.debug(f"Using Dockerfile: {image}.docker")
        run_command(
            ["docker", "build", "-f", f"{image}.docker", "-t", build_image_name, "."]
        )
        logger.info(f"Successfully built image: {build_image_name}")

    # Push images if registry is specified
    if registry:
        logger.info(
            f"Registry specified ({formatted_registry}), proceeding with image push"
        )
        for image in images:
            push_image_name: str = f"{formatted_registry}{image}{formatted_tag}"
            logger.info(f"Pushing image: {push_image_name}")
            run_command(["docker", "push", push_image_name])
            logger.info(f"Successfully pushed image: {push_image_name}")
    else:
        logger.info("No registry specified, skipping image push")

    logger.info("Docker image build process completed successfully")


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="Build and push Docker images"
    )
    parser.add_argument("--registry", help="Docker registry URL", default="")
    parser.add_argument("--tag", help="Image tag", default="")
    parser.add_argument("--debug", action="store_true", help="Enable debug logging")
    parser.add_argument(
        "--images",
        nargs="+",
        help="List of images to build and push. If not specified, all images will be built.",
        default=DOCKER_IMAGES,
    )

    args: argparse.Namespace = parser.parse_args()

    # Set debug logging if requested
    if args.debug:
        logger.setLevel(logging.DEBUG)
        logger.debug("Debug logging enabled")

    try:
        build_and_push_images(args.registry, args.tag, args.images)
    except subprocess.CalledProcessError as e:
        logger.error(f"Build process failed with exit code {e.returncode}")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Unexpected error during build process: {str(e)}")
        sys.exit(1)


if __name__ == "__main__":
    main()
