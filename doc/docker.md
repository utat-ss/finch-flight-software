## Docker container structure

The Docker container has a minimal Ubuntu 24.04 installation under the hood. It's files are structured in the following way:

|Path|Description|
|----|-----------|
|`/build`|Folder with build artifacts|
|`/west.yml`|Config file used by west commands|
|`/workspace`|Mounted finch-flight-software project folder|
|`/zephyr`|Zephyr Installation|
|`/zephyr-sdk`|Zephyr SDK Installation|
|`/env-setup.sh`|Activates Zephyr environment and Python venv|
|`.venv`|Python virtual environment containing west|
|`/entrypoint.sh`|Entrypoint script|

### Troubleshooting the container

If the container doesn't open or it has some other issue, you can try removing it and opening it again.

Run the following commands from the host terminal:

1. `docker container rm utat-ss-finch-container`
2. `./scripts/open_container.sh` or `.\scripts\open_container.ps1`

### Updating the container

Run the following commands from the host terminal:

1. `docker container stop utat-ss-finch-container`
2. `docker container rm utat-ss-finch-container`
3. `docker image rm ghcr.io/utat-ss/finch-flight-software`
4. `docker pull ghcr.io/utat-ss/finch-flight-software:latest`
5. `./scripts/open_container.sh` or `.\scripts\open_container.ps1`
