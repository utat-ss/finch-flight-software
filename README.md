# Getting Started

This guide covers setup on **Ubuntu 24.04 LTS** with **x86_64** processor.

### System Update

```sh
sudo apt update
sudo apt upgrade
```

### Make an Empty Directory

```sh
mkdir ~/finch
cd ~/finch
```

**Note:** This is because the west manifest will populate one directory above this repository.

### Clone This Repository

Using **HTTPS**:

```sh
git clone https://github.com/utat-ss/finch-flight-software.git
cd finch-flight-software
```

Using **SSH**:

```sh
git clone git@github.com:utat-ss/finch-flight-software.git
cd finch-flight-software
```

### Install Dependencies

```sh
sudo ./scripts/install_dependencies.sh
```

### Set Up Python Virtual Environment

```sh
./scripts/setup_python_venv.sh
```

### Set Up West Workspace

```sh
./scripts/setup_west_workspace.sh
```

### Set Zephyr Environment Variables
```sh
source ../zephyr/zephyr-env.sh
```
### Activate Python Virtual Environment
```sh
source ../.venv/bin/activate
```
