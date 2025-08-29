# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

$ContainerName = "utat-ss-finch-container"
$ImageName = "ghcr.io/utat-ss/finch-flight-software"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

function ContainerExists {
    docker ps -a --format '{{.Names}}' | Where-Object { $_ -eq $ContainerName } | ForEach-Object { return $true }
    return $false
}

function ContainerRunning {
    docker ps --format '{{.Names}}' | Where-Object { $_ -eq $ContainerName } | ForEach-Object { return $true }
    return $false
}

if (ContainerRunning) {
    Write-Host "Container '$ContainerName' is running. Opening a new shell."
    docker exec -it $ContainerName /bin/bash -c "/entrypoint.sh"
}
elseif (ContainerExists) {
    Write-Host "Container '$ContainerName' exists but is not running. Starting."
    docker start $ContainerName
    docker attach $ContainerName
}
else {
    Write-Host "Container '$ContainerName' does not exist. Running a new container."
    $MountPath = Resolve-Path "$ScriptDir\.."
    docker run -it -v "${MountPath}:/workspace" --add-host="host.docker.internal:host-gateway" --name $ContainerName $ImageName
}
