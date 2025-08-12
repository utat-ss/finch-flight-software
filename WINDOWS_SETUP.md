# Windows Setup

This setup guide uses Docker and **Powershell**. **Please don't run the commands in WSL or Git Bash or cmd.**

If you want to proceed with the manual setup in WSL instead, please take a look at `MANUAL_SETUP.md` for more details.

## Installation

### 0. Dependencies

Make sure `python` and `git` are installed **and are available from Powershell**.

Install Python 3 using [this](https://www.python.org/downloads/) link. Select option 'Add Python to PATH' in the installer.

To install Git, use [this](https://git-scm.com/downloads/win) link. Git will be added to PATH by default.

### 1. Clone the repository

Using **SSH**:

```sh
git clone git@github.com:utat-ss/finch-flight-software.git
cd finch-flight-software
```

Or using **HTTPS**:

```sh
git clone https://github.com/utat-ss/finch-flight-software.git
cd finch-flight-software
```

### 2. Docker

Install [Docker Desktop](https://www.docker.com/products/docker-desktop/).

### 3. Pyocd

If your computer uses x86, run `python -m pip install -U pyocd` in Powershell.
For non-x86 systems, use [this](https://pyocd.io/docs/installing_on_non_x86.html) guide.

### 4. Pyocd packs

Pyocd needs Keil packs to flash the code to stm32 boards.

#### 4.0. Create a folder

* Create folder named `packs` in the project root.

#### 4.1. Download pyocd pack for stm32h7

* Open [this](https://www.keil.arm.com/packs/stm32h7xx_dfp-keil/overview/) page or search `stm32h7 pack keil`.
* Download the .pack file and place it in the `packs` folder.
* Rename the .pack file to `h7.pack`.

#### 4.2. Download pyocd pack for stm32g4

* Open [this](https://www.keil.arm.com/packs/stm32g4xx_dfp-keil/overview/) page or search `stm32g4 pack keil`.
* Download the .pack file and place it in the `packs` folder.
* Rename the .pack file to `g4.pack`.

### 5. Pull the FINCH dev environment image

In the Powershell terminal:
```sh
docker pull ghcr.io/utat-ss/finch-flight-software:latest
```

This command pulls the dev environment with Zephyr and Zephyr SDK installed. It might take a few minutes to finish.

## Usage

Building commands are done from the Docker container. You will need another terminal to flash or debug the board.

### 1. Open the container

Run `.\scripts\open_container.ps1` in Powershell.

This script will open the terminal of the container, where you can do the `west` commands. The container doesn't have access to USB devices.

### 2. Build the code

Once you are in the container, you can run `west build`.

Example: `west build -p always -b nucleo_h753zi apps/pay` will build the `pay` app for our H7 dev board.

The build artifact is in `/build` directory in the container.

### 3. Flash or debug the board

#### 3.0. Run the pyocd server

Keep the terminal with the Docker container open. Open another Powershell terminal.

If you are flashing to the H7 board, run `.\scripts\run_pyocd_server_h7.ps1`.

If you are flashing to the G4 board, run `.\scripts\run_pyocd_server_g4.ps1`.


These scripts put the chip in the debugging mode and make it available. You need to run the server on the host every time you want to flash or debug the board.

#### Flash

In the container, run `./scripts/flash_from_container.sh`. This script opens GDB, connects it to the server and runs the flashing commands.

The GDB commands for flashing are specified in the `scripts/gdb_commands_flash` file.

#### Debug

In the container, run `./scripts/debug_from_container.sh`. This script opens GDB and connects it to the server.

The GDB commands for debugging are specified in the `scripts/gdb_commands_debug` file.
