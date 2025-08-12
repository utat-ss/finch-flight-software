# Setup

This setup guide uses Docker. Our Docker container supports building the Zephyr applications, as well as flashing and debugging the boards.

The supported platforms are Windows/x86_64, macOS/aarch64, Linux/x86_64. Other platforms are likely to work, but installing pyOCD might need extra steps.

>> **On Windows, this guide uses Powershell as a host terminal. Please don't run the commands in WSL or Git Bash or cmd.**

If you use Linux/WSL and want to proceed with the manual setup instead, please take a look at `MANUAL_SETUP.md` for more details.

## Installation

### 1. Install Git, Python

Git is used to track and manage changes of our code. Python is used in the pyOCD debugger as well as few other scripts and tools.

<details><summary>Windows</summary>

Make sure `python` and `git` are installed **and are available from Powershell**.

You can use [this](https://www.python.org/downloads/) link to install Python 3. Select option 'Add Python to PATH' in the installer.

To install Git, use [this](https://git-scm.com/downloads/win) link. Git will be added to PATH by default.

</details>

<details><summary>macOS</summary>

Before proceeding, make sure `python3` and `git` commands are available in the terminal.

Python and git can be installed with homebrew:
```
brew install git
brew install python
```

</details>

<details><summary>Linux</summary>

Before proceeding, make sure `python3` and `git` commands are available in the terminal.

On Ubuntu:
```
sudo apt update
sudo apt install python3 python3-pip
```

On Fedora:
```
sudo dnf install python3 git
```

</details>

### 2. Clone the repository

You can clone the repo using SSH or HTTPS. If you want to push commits to Github, you must use SSH.


<details><summary>SSH</summary>

In the host terminal, open a folder with development projects. Then run:
```sh
git clone git@github.com:utat-ss/finch-flight-software.git
cd finch-flight-software
```
</details>

<details><summary>HTTPS</summary>

In the host terminal, open a folder with development projects. Then run:
```sh
git clone https://github.com/utat-ss/finch-flight-software.git
cd finch-flight-software
```

</details>


### 3. Install pyOCD

pyOCD is a tool that facilitates the interaction between the STM32 chip and your computer.

In the host terminal, run:

___________

<details><summary>Windows</summary>

`python -m pip install -U pyocd`

</details>

<details><summary>macOS/Linux</summary>

`python3 -m pip install -U pyocd`

</details>

___________


If you have a Raspberry Pi or non-x86 Windows platform to install pyOCD on, check out [this](https://pyocd.io/docs/installing_on_non_x86) pyOCD guide for more information.

### 4. Download and place pyOCD packs

pyOCD needs Keil packs for STM32G4 and STM32H7 to support our OBC and PAY boards. When you run any pyOCD command, you can pass a Keil pack with a `--pack` option. Our scripts also use `--pack` option and expect the packs under a specific path.

Steps:

1. Create folder named `packs` in the `finch-flight-software` folder.
2. Open [this](https://www.keil.arm.com/packs/stm32h7xx_dfp-keil/overview/) page or search `stm32h7 pack keil`.
3. Download the .pack file and place it in the `packs` folder.
4. Rename the .pack file to `stm32h7.pack`.
5. Open [this](https://www.keil.arm.com/packs/stm32g4xx_dfp-keil/overview/) page or search `stm32g4 pack keil`.
6. Download the .pack file and place it in the `packs` folder.
7. Rename the .pack file to `stm32g4.pack`.


### 5. Install Docker

Docker is a tool that can run containers (sandboxes) on your machine. We use Docker containers so that everyone has the same development environment.

___________

<details><summary>Windows</summary>

Install Docker Desktop using [this](https://www.docker.com/products/docker-desktop/) link.

After installing, open the Docker Desktop application and keep it running in the background. You can skip the account creation.

</details>

<details><summary>macOS</summary>

For macOS, install [Docker Desktop](https://www.docker.com/products/docker-desktop/).

After installing, open the Docker Desktop application and keep it running in the background. You can skip the account creation.

</details>

<details><summary>Linux</summary>

Please use [this](https://docs.docker.com/engine/install/) guide to install Docker.
</details>

___________

### 6. Pull the FINCH dev environment image

In the host terminal, run:
```sh
docker pull ghcr.io/utat-ss/finch-flight-software:latest
```

This command pulls the container with our development environment. It might take a few minutes to finish.

## Usage

Our Docker container supports building the code by itself. To flash/debug the board, you need both the host and the container terminal.

The container doesn't have proper access to SSH credentials. For this reason, you need to run `git` commands on the host.

|Use case|Needed terminal|
|--------|--------|
|Build|Container only|
|Flash/debug the board|Both container and host|
|git|Host only|
|View the board output|Host only|

### 1. Open the container

In the host terminal, open the `finch-flight-software` folder. Then, run the `open_container` script:

__________

<details><summary>Windows</summary>

`.\scripts\open_container.ps1`

</details>

<details><summary>macOS/Linux</summary>

`./scripts/open_container.sh`

</details>

___________

This script will open the terminal of the container, where you can run the `west` commands. You can run `exit` command to come back to the host terminal.

### 2. Build the code

Once you are in the container, you can run `west build`.

Example: `west build -p always -b nucleo_h753zi apps/pay` will build the `pay` app for our H7 dev board.

The build artifact is in the `/build` directory of the container.

### 3. Flash the board

#### 3.1. Run the GDB server on host

Keep the terminal with the Docker container open. Open another terminal on the host. Open the `finch-flight-software` folder, and run the server script:

___________

<details><summary>Windows</summary>

`.\scripts\run_pyocd_server.ps1 <TARGET>`

</details>

<details><summary>macOS/Linux</summary>

`./scripts/run_pyocd_server.sh <TARGET>`

</details>

___________

Put your target chip instead of `<TARGET>`, like `stm32h753zitx`. You can omit `<TARGET>` on the Nucleo boards.

The script just runs `pyocd gdbserver` commands with a few extra arguments. It puts the chip in the debugging mode and makes it available. To stop the GDB server, press CTRL+C in the terminal with server running.

#### 3.2. Run the flash script from Docker

In the container, run:
```
./scripts/flash_from_container.sh
```

This script opens GDB, connects it to the server and runs the flashing commands. The GDB commands for flashing are specified in the `scripts/gdb_commands_flash` file.

GDB will wait until the server is available.

### 4. Debug the board

#### 4.1. Run the GDB server on host

In the host terminal, run:
___________

<details><summary>Windows</summary>

`.\scripts\run_pyocd_server.ps1 <TARGET>`

</details>

<details><summary>macOS/Linux</summary>

`./scripts/run_pyocd_server.sh <TARGET>`

</details>

___________

See 3.1 for more details.

#### 4.2. Run the debug script from Docker

In the container, run:
```
./scripts/debug_from_container.sh
```

This script opens GDB and connects it to the server, it doesn't do anything else. The GDB commands are in the `scripts/gdb_commands_debug` file.

### 5. View the board output

<details><summary>Windows</summary>

You can use a tool like puTTY/Tera Term/Realterm or others. Make sure the baud rate is at least 115200.

</details>

<details><summary>macOS</summary>

You can use minicom to view the board output. You can install minicom with:
```sh
brew install minicom
```

</details>

<details><summary>Linux</summary>

You can use minicom to view the board output. On Ubuntu, minicom can be installed with:
```sh
sudo apt-get install minicom
```

On Fedora:
```sh
sudo dnf install minicom
```

</details>
