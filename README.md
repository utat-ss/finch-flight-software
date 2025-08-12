# Getting Started

## Setup

The installation and usage instructions are in `LINUX_MACOS_SETUP.md` for Linux/macOS, and in `WINDOWS_SETUP.md` for Windows.

If you use Ubuntu 24.04 or WSL and want to install everything manually, use `MANUAL_SETUP.md`.

## Docker container structure

The Docker container has a minimal Ubuntu 24.04 installation under the hood. It's files are structured in the following way:
```
    /build      -- Folder with build artifacts
    /west.yml   -- Config file used by west commands
    /workspace  -- Mounted finch-flight-software project folder
    /zephyr     -- Zephyr Installation
    /zephyr-sdk -- Zephyr SDK Installation
```
