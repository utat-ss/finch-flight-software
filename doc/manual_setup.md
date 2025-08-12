## Manual Setup (only Ubuntu/WSL)

This guide is for **Ubuntu 24.04 LTS**. It also works on Ubuntu 24.04 running in WSL. This manual guide might not work on other operating systems.

#### 1. Update the System:

```sh
sudo apt update
sudo apt upgrade
```

#### 2. Make an Empty Directory

```sh
mkdir ~/finch
cd ~/finch
```
**Note:** This is because the west manifest will populate one directory above this repository.

#### 3. Clone This Repository

Using **HTTPS**:

```sh
git clone https://github.com/utat-ss/finch-flight-software.git
cd finch-flight-software
```

Or using **SSH**:

```sh
git clone git@github.com:utat-ss/finch-flight-software.git
cd finch-flight-software
```

#### 4. Install Dependencies

```sh
sudo ./scripts/install_dependencies.sh
```

#### 5. Set Up Python Virtual Environment

```sh
./scripts/setup_python_venv.sh
```

#### 6. Set Up West Workspace

```sh
./scripts/setup_west_workspace.sh
```

On this step the Zephyr SDK is installed, so it might take a while.

### Steps to do before using the manual environment

Every time you open a terminal to build the apps, you need to activate the virtual environment and set the Finch Flight Software environment variable.

#### 1. Set Zephyr Environment Variables
```sh
source ../zephyr/zephyr-env.sh
```

#### 2. Activate Python Virtual Environment
```sh
source ../.venv/bin/activate
```

### Flashing from the manual environment

We suppose that the board is available through USB.

To flash the board, run the `west flash` command. To select a different runner, use the `-r` argument. `pyocd` is the recommended runner.
