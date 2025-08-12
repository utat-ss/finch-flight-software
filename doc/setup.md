# Setup

This setup guide uses Docker. Our Docker container supports building the Zephyr applications, as well as flashing and debugging.

The supported platforms are Windows/x86_64, macOS/aarch64, Linux/x86_64. Other platforms are likely to work, but installing pyOCD might need extra steps.

>> **On Windows, this guide uses Powershell as a host terminal. Please don't run the commands in WSL or Git Bash or cmd.**

If you use Linux/WSL and want to proceed with the manual setup instead, please take a look at `doc/manual_setup.md` for more details.

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

You can clone the repo using SSH or HTTPS.

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

pyOCD is a Python tool for programming and debugging various microcontrollers, including those with Arm Cortex-M chips.

In the host terminal, run:

```
python3 -m pip install -U pyocd
```

You might need to use `python` or `py` command instead of `python3`. You can install pyOCD in a Python virtual environment or in a conda environment, but then you need to activate it first before running the GDB server script.

Try running `pyocd` command afterwards to see if it works. For Windows it might not work right away, in that case you need to add pyOCD to the PATH environment variable. Carefully look at the instllation logs from pip to find the path for `pyocd` and add it to the PATH.

If you have a non-x86 Windows/Linux platform to install pyOCD on, check out [this](https://pyocd.io/docs/installing_on_non_x86) pyOCD guide for more information.

### 4. Install Docker

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

### 5. Pull the FINCH dev environment image

In the host terminal, run:
```sh
docker pull ghcr.io/utat-ss/finch-flight-software:latest
```

This command pulls the container with our development environment. It might take a few minutes to finish.

## Usage

Our Docker container supports building the code by itself. To flash or debug, you need both the host and the container terminal.

It is technically possible to setup git and SSH/HTTPS access so that you can push code from the container, however the setup process is more complicated and depends on the setup you have. For this reason, we run `git` commands on the host.

|Use case|Needed terminal|
|--------|--------|
|Build|Container only|
|Flash, debug|Both container and host|
|git|Host only|
|View the logs|Host only|

### 0. Docker Engine

If you're using macOS or Windows, make sure the Docker Desktop app is open (in the background is fine too). Otherwise the Docker Engine won't be running and the container won't open.

If you're using Linux, you can proceed with the next step.

### 1. Open the container

In the host terminal, open the `finch-flight-software` folder. Then, run the `container_open` script:

__________

<details><summary>Windows</summary>

```powershell
.\scripts\container_open.ps1
```

Powershell might now allow to run the script for the first time. To resolve this, run:

```
Unblock-File -Path .\scripts\container_open.ps1
```

This command also works:
```
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```

</details>

<details><summary>macOS/Linux</summary>

```sh
./scripts/container_open.sh
```

</details>

___________

This script will open the terminal of the container, where you can run the `west` commands. You can run `exit` command to come back to the host terminal.

### 2. Build

Once you are in the container, you can run `west build`.

Example: `west build -p always -b nucleo_h753zi apps/pay` will build the `pay` app for our H7 dev board.

The build artifact is in the `/build` directory of the container.

### 3. Flash

#### 3.1. Run the GDB server on host

Keep the terminal with the Docker container open. Open another terminal on the host. Open the `finch-flight-software` folder, and run the server script:

___________

<details><summary>Windows</summary>

```powershell
.\scripts\gdb_server.ps1 <TARGET>
```

</details>

<details><summary>macOS/Linux</summary>

```sh
./scripts/gdb_server.sh <TARGET>
```

</details>

___________

Put your target chip instead of `<TARGET>`, like `stm32h753zitx`. You can omit `<TARGET>` on the Nucleo boards.

The script just runs `pyocd gdbserver` commands with a few extra arguments. It puts the chip in the debugging mode and makes it available. To stop the GDB server, press CTRL+C in the terminal with server running.

#### 3.2. Run the flash script from Docker

In the container, run:
```sh
./scripts/container_flash.sh
```

This script opens GDB, connects it to the server and runs the flashing commands. The GDB commands for flashing are specified in the `scripts/gdb_commands_flash` file.

GDB will wait until the server is available.

### 4. Debug

#### 4.1. Run the GDB server on host

In the host terminal, run:
___________

<details><summary>Windows</summary>

```powershell
.\scripts\gdb_server.ps1 <TARGET>
```

</details>

<details><summary>macOS/Linux</summary>

```sh
./scripts/gdb_server.sh <TARGET>
```

</details>

___________

See 3.1 for more details.

#### 4.2. Run the debug script from Docker

In the container, run:
```sh
./scripts/container_debug.sh
```

This script opens GDB and connects it to the server, it doesn't do anything else. The GDB commands are in the `scripts/gdb_commands_debug` file.

### 5. View the logs

<details><summary>Windows</summary>

You can use a tool like puTTY/Tera Term/Realterm or others. The most commonly used baud rate is 115200, which is often not the baud rate set in those tools. Make sure to set the proper rate in the tool.

</details>

<details><summary>macOS</summary>

You can use minicom to view the logs. You can install minicom with:
```sh
brew install minicom
```

</details>

<details><summary>Linux</summary>

You can use minicom to view the logs. On Ubuntu, minicom can be installed with:
```sh
sudo apt-get install minicom
```

On Fedora:
```sh
sudo dnf install minicom
```

</details>
