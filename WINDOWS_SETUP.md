# Windows Setup

This setup guide uses Docker and **Powershell**. **Please don't run the commands in WSL or Git Bash or cmd.**

If you want to proceed with the manual setup in WSL instead, please take a look at `MANUAL_SETUP.md` for more details.

## Installation

### 0. Dependencies

Make sure `python` and `git` are installed **and are available from Powershell**.

Install Python 3 using [this](https://www.python.org/downloads/) link. Select option 'Add Python to PATH' in the installer.

To install Git, use [this](https://git-scm.com/downloads/win) link. Git will be added to PATH by default.

### 1. Clone the repository

You can clone the repo using SSH or HTTPS. If you want to push commits to Github, you must use SSH.

**SSH**:

```sh
git clone git@github.com:utat-ss/finch-flight-software.git
cd finch-flight-software
```

**HTTPS**:

```sh
git clone https://github.com/utat-ss/finch-flight-software.git
cd finch-flight-software
```

### 2. Docker

Docker is a tool that can run containers (sandboxes) on your machine. We use Docker containers so that everyone has the same development environment.

Install Docker Desktop from [this](https://www.docker.com/products/docker-desktop/) link.

### 3. pyOCD

pyOCD is a tool that facilitates the interaction between the STM32 chip and your computer.

If your computer uses x86 CPU, run `python -m pip install -U pyocd` in Powershell.
For non-x86 systems, use [this](https://pyocd.io/docs/installing_on_non_x86.html) guide.

### 4. pyOCD packs

pyOCD needs Keil packs for STM32G4 and STM32H7 to support our OBC and PAY boards. When you run any pyOCD command, you can pass a Keil pack with a `--pack` option. Our scripts also use `--pack` option and expect the packs under a specific path.

#### 4.0. Create a folder

* Create folder named `packs` in the project root.

#### 4.1. Download the pyOCD pack for STM32H7

* Open [this](https://www.keil.arm.com/packs/stm32h7xx_dfp-keil/overview/) page or search `stm32h7 pack keil`.
* Download the .pack file and place it in the `packs` folder.
* Rename the .pack file to `stm32h7.pack`.

#### 4.2. Download the pyOCD pack for STM32G4

* Open [this](https://www.keil.arm.com/packs/stm32g4xx_dfp-keil/overview/) page or search `stm32g4 pack keil`.
* Download the .pack file and place it in the `packs` folder.
* Rename the .pack file to `stm32g4.pack`.

### 5. Pull the FINCH dev environment image

In the Powershell terminal:
```sh
docker pull ghcr.io/utat-ss/finch-flight-software:latest
```

This command pulls the dev environment with Zephyr and Zephyr SDK installed. It might take a few minutes to finish.

## Usage

You can build the code from the Docker container.

Because our Docker container don't have USB access, you will need two terminals to flash or debug the board: one inside the container, and one on the host.

The container doesn't have proper access to SSH credentials too. For this reason, you need to run `git` commands on the host.

### 1. Open the container

Run `.\scripts\open_container.ps1` in Powershell.

This script will open the terminal of the container, where you can do the `west` commands. The container doesn't have access to USB devices.

### 2. Build the code

Once you are in the container, you can run `west build`.

Example: `west build -p always -b nucleo_h753zi apps/pay` will build the `pay` app for our H7 dev board.

The build artifact is in the `/build` directory in the container.

### 3. Flash the board

Steps:

1. Run the pyOCD server on the host
2. Run the flash script from Docker

#### 3.1. Run the pyOCD server

Keep the terminal with the Docker container open. Open another Powershell terminal and `cd` to the `finch-flight-software` folder.

Run `.\scripts\run_pyocd_server.ps1 --target=...`. Instead of `...` put your target chip, like `stm32h753zitx`.

The script just runs `pyocd gdbserver` with a few extra arguments. It puts the chip in the debugging mode and makes it available. You need to run the server on the host every time you want to flash or debug the board.

#### 3.2. Flash the board

In the container, run `./scripts/flash_from_container.sh`.

This script opens GDB, connects it to the server and runs the flashing commands. The GDB commands for flashing are specified in the `scripts/gdb_commands_flash` file.

GDB will wait until the server is available.

### 4. Debug the board

Steps are similar to flashing:

1. Run the pyOCD server on the host
2. Run the debug script (`./scripts/debug_from_container.sh`)

#### 4.1. Run the pyOCD server

See 3.1 for more details.

#### 4.2. Debug the board

In the container, run `./scripts/debug_from_container.sh`.

This script opens GDB and connects it to the server, it doesn't do anything else. The GDB commands are in the `scripts/gdb_commands_debug` file.
