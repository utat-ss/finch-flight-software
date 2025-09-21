# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$G4Pack = "$ScriptDir\..\blobs\Keil.STM32G4xx_DFP.2.1.0.pack"
$H7Pack = "$ScriptDir\..\blobs\Keil.STM32H7xx_DFP.4.1.2.pack"

if ($args.Count -ne 0 -and $args.Count -ne 1) {
    Write-Host "Error: must pass 0 or 1 arguments"
    Write-Host "Usage: $($MyInvocation.MyCommand.Name) [TARGET]"
    exit 1
}

if ($args.Count -eq 0) {
    Write-Host "  WARNING: running the gdb server without the target argument." -ForegroundColor Yellow
    Write-Host "  This will only work on the Nucleo dev boards." -ForegroundColor Yellow
    Write-Host "  To add the target, run $($MyInvocation.MyCommand.Name) <TARGET>" -ForegroundColor Yellow
    Write-Host "Starting in 3s.."
    Start-Sleep -Seconds 3
}

while ($true) {
    if ($args.Count -eq 0) {
        pyocd gdbserver --connect attach --allow-remote --pack ${G4Pack} --pack ${H7Pack}
    }
    elseif ($args.Count -eq 1) {
        pyocd gdbserver --target $args[0] --connect attach --allow-remote --pack ${G4Pack} --pack ${H7Pack}
    }

    if ($LastExitCode -ne 0) {
        exit 1
    }

    Write-Host "pyOCD exited, restarting in 2s... (Ctrl+C to stop)"
    Start-Sleep -Seconds 2
}
