#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

CONTAINER_NAME=utat-ss-finch-container
IMAGE_NAME=ghcr.io/utat-ss/finch-flight-software
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

container_exists() {
    docker ps -a --format '{{.Names}}' | grep -w "$CONTAINER_NAME" > /dev/null 2>&1
}

container_running() {
    docker ps --format '{{.Names}}' | grep -w "$CONTAINER_NAME" > /dev/null 2>&1
}

if container_running; then
    echo "Container '$CONTAINER_NAME' is running. Opening a new shell."
	docker exec -it "$CONTAINER_NAME" /bin/bash -c "/entrypoint.sh"
elif container_exists; then
    echo "Container '$CONTAINER_NAME' exists but is not running. Starting."
    docker start "$CONTAINER_NAME"
    docker attach "$CONTAINER_NAME"
else
    echo "Container '$CONTAINER_NAME' does not exist. Running a new container."
    docker run -it -v "$SCRIPT_DIR"/..:/workspace:z --add-host="host.docker.internal:host-gateway" --name "$CONTAINER_NAME" "$IMAGE_NAME"
fi
