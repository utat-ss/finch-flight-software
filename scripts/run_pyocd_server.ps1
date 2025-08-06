# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

if ($args.Count -ne 0 -and $args.Count -ne 1) {
    Write-Host "Error: must pass 0 or 1 arguments"
    Write-Host "Usage: $($MyInvocation.MyCommand.Name) [TARGET]"
    exit 1
}

while ($true) {
    if ($args.Count -eq 0) {
        pyocd gdbserver --allow-remote --pack "$ScriptDir\..\packs\stm32g4.pack" --pack "$ScriptDir\..\packs\stm32h7.pack"
    }
    elseif ($args.Count -eq 1) {
        pyocd gdbserver --allow-remote --pack "$ScriptDir\..\packs\stm32g4.pack" --pack "$ScriptDir\..\packs\stm32h7.pack" $args[0]
    }

    if ($LastExitCode -ne 0) {
        exit 1
    }

    Write-Host "pyOCD exited, restarting in 2s... (Ctrl+C to stop)"
    Start-Sleep -Seconds 2
}
