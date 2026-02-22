# Flatsat Development Guide

This document provides instructions to start developing on the Flatsat via the Raspberry Pi. Due to the storage limitations of the Pi, we share the same Zephyr workspace across multiple users. However, this makes the instructions slightly convoluted.

## Prerequisites
This guide assumes that you already have VPN access and are SSHed into the Raspberry Pi as the `utatpi` user.

## Making your own user
To avoid conflicts and to allow you to customize your own development environment, all developers should work from their own user rather than from `utatpi`. To create a new user, do the following:

Create your user. Set a password when prompted. You can leave the other fields blank by just pressing enter:
```bash
sudo adduser <USERNAME>
```

Add the newly created user to the sudo group:
 ```bash
 sudo adduser <USERNAME> sudo
 ```

Allow the new user to access the utatpi home directory, which contains the firmware directory
```bash
sudo setfacl -m u:<USERNAME>:rx /home/utatpi
````

Switch to this new user:
```bash
sudo su <USERNAME>
```

Now next time you SSH onto the pi, you can directly log in to your user:
```bash
ssh <USERNAME>@10.0.1.85
```

## Cloning the repository  
Generate SSH keys for the user. Follow the prompts, the defaults are fine. Make note of the file the key is saved in:
```bash
ssh-keygen
```

Find and copy the public key created from the previous step (this is the file you entered in the last step, but with the `.pub` file extension. E.g. `~/.ssh/id_ed25519.pub`).

Now go to [GitHub](https://github.com) and add the public key to your account by clicking on your profile -> settings -> SSH and GPG keys -> New SSH key, and paste in the key.

Navigate to the firmware directory:
```bash
cd /home/utatpi/firmware
```

Clone the repository:
```bash
git clone git@github.com:utat-ss/finch-flight-software.git <USERNAME>
```

Your repository is now cloned to `/home/utatpi/firmware/<USERNAME>`.

In the future, to prevent needing to traverse back to `/home/utatpi`, you can create a symlink to your home directory:
```bash
ln -s /home/utatpi/firmware ~
```
Now, you can access the firmware repository directly at `~/firmware`.
## Setting up the repository
Your shell does not know the path of the Zephyr SDK, since it was installed on the `utatpi` user. To specify the path, you need to modify your shell's startup file. By default, you are using Bash, so the script is located at `~/.bashrc`. This will be different if you use a different shell. Append the following to the end of it:
 ```bash

 export ZEPHYR_SDK_INSTALL_DIR=~/firmware/zephyr-sdk
 ``` 
 
The Zephyr workspace needs to know where your project is located (where you cloned the `finch-flight-software` repository to). Add the project path (relative to `~/firmware`) to `~/.westconfig` (you may need to create the file):
```
[manifest]
path = <USERNAME>
```

For your changes in this section to apply, you will need to restart your shell or run the following (depending on your shell) to apply the changes:
```bash
source ~/.bashrc
```

Return back to the firmware directory. To give yourself the commands for the workspace, you need to enable the Python virtual environment. You need to do this every time you log in:
```bash
source ~/firmware/.venv/bin/activate
```

Enter `~/firmware` and update west. Currently, the command may fail with `ERROR: update failed for projects: libcsp, cmsis_6, hal_stm32`. This is a separate error and can be safely ignored. However, if you get an error like `FATAL ERROR: command exited with status 128: remote add -- origin https://github.com/zephyrproject-rtos/zephyr.git`, just run the recommended command given in the error message:
```bash
west update
```

To enable PyOCD support for the `stm32g431rbtx` and `stm32h753zitx` (the chips we are currently using. Feel free to include other chips as required), run the following:
```bash
pyocd pack install stm32g431rbtx stm32h753zitx
```
## Usage  
Building and flashing work the same as usual. Run these commands from `~/firmware/<USERNAME>`. Make sure you are in the Python Virtual Environment (see above).

### Build
```bash
west build -p always -b <YOUR_BOARD> ./apps/<YOUR_APP>/
```

### Flash
```bash
west flash -r pyocd
```

If you get a permission error when flashing, you may need to give yourself permission to access the board's file directory:
```bash
chmod -R 777 /media/utatpi/<DIRECTORY>
```
