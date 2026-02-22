# Flatsat Development Guide

This document provides instructions to start developing on the flatsat via the Raspberry Pi. Due to the storage limitations of the Pi, we build the image on the local machine and use `scp` to copy the built binary onto the pi. We then flash the binary from the pi onto the desired board.

## Prerequisites
This guide requires that you are able to build the firmware on your local machine. The instructions for building the firmware can be found in the [manual_setup.md](manual_setup.md) and [setup.md](setup.md) guides (As long as you have completed one of the two it's ok).

## Connecting to the pi

To connect to the pi, you first need to ask the current firmware team lead to add your SSH key to the pi and help you setup the VPN configuration. Once that is done, you can connect to the pi using the following command:

```bash
ssh utatpi@10.0.1.85
```

## Building the firmware

If you followed the manual setup, you should know how to build the firmware onto a `build` directory. If so, you can skip to the next section.

However, if you followed the regular setup guide, you cannot build the firmware the usual way because the build destination (`/build`) is in the docker container and not accessible from outside of the container. To build the firmware, you need to add a `-d /workspace/build` flag to your usual `west build` command. E.g.

```bash
west build -p always -b obc apps/obc -d /workspace/build
```

You should now be able to see a `build` directory in the root of the `finch-firmware` repository (outside of the docker container).

## Making your own user
To avoid conflicts and to allow you to customize your own development environment, **all developers should work from their own user rather than from `utatpi`**. To create a new user, do the following:

Create your user. Set a password when prompted. You can leave the other fields blank by just pressing enter:
```bash
sudo adduser <USERNAME>
```

Add the newly created user to the sudo group:
 ```bash
 sudo adduser <USERNAME> sudo
 ```

Switch to this new user:
```bash
sudo su <USERNAME>
```

Now next time you SSH onto the pi, you can directly log in to your user:
```bash
ssh <USERNAME>@10.0.1.85
```

We now setup the virtual environment and PyOCD support for the new user.

Create a Python virtual environment for the new user. This is where we will install PyOCD and other dependencies:
```bash
python3 -m venv venv
```

Enable the virtual environment:
```bash
source venv/bin/activate
```

Install PyOCD in the virtual environment:
```bash
pip3 install pyocd
```

To enable PyOCD support for the `stm32g431rbtx` and `stm32h753zitx` (the chips we are currently using. Feel free to include other chips as required), run the following:
```bash
pyocd pack install stm32g431rbtx stm32h753zitx
```

## Usage  

### Copying the built binary to the pi
To flash the firmware, we first need to copy the built binary from the local machine to the pi. We can use `scp` for this. The command is as follows (run from the root of the `finch-firmware` repository on your local machine):

```bash
scp build/zephyr/zephyr.hex <YOUR_USERNAME>@10.0.1.85:~/zephyr.hex
```

Now on the pi, you should see the `zephyr.hex` file in your home directory. You can now flash this binary onto the desired board using PyOCD.

Make sure you are in the Python Virtual Environment (see the `source` command above).

### Flash
```bash
pyocd flash --target <TARGET> zephyr.hex
```

Where `<TARGET>` is the target board you want to flash. E.g. `stm32g431rbtx` or `stm32h753zitx`.

### Logs

To view the logs from the board, **do not** use `minicom` or `screen`, as only one user can use it at a time. Instead, we have defined systemd services that will copy the logs to files. You can read these log files at `/var/log/<BOARD_NAME>.log`. E.g. `/var/log/obc.log` for the OBC board. You can see the list of available log files by running `ls /var/log/`. To view the logs in real time, you can use the `tail` command:

```bash
tail -f /var/log/<BOARD_NAME>.log
```

### Debugging

To debug the firmware, we start a GDB server on the pi using PyOCD, and then connect to it using GDB from the local machine.

On the pi, run the following command to start the GDB server:

```bash
pyocd gdbserver --allow-remote
```

This will start a GDB server. Note the port number that the server is running on (default is 3333).

If you used the docker setup, open the container and run the following command from inside the container:

```bash
/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb build/zephyr/zephyr.elf -ex "target remote 10.0.1.85:<PORT_NUMBER>"
```

If you are not, `zephyr-sdk` will be in a different directory, so adjust the path to `arm-zephyr-eabi-gdb` accordingly and run the same command.