1. **Install Dependencies (Ubuntu)**:
    ```sh
    sudo apt-get update
    sudo apt-get install -y openocd usbutils minicom  # openocd (flashing), usbutils (lsusb), minicom (logs)
    ```

2. **USB Passthrough to WSL (required for flashing)**:
    - **Windows (Powershell)**:
        ```powershell
        usbipd list # Look for STMicroelectronic
        usbipd share --wsl --busid <busid> # Replace with your Nucleo's busid
        usbipd attach --wsl --busid <busid>
        ```
    - **Ubuntu**:
        ```sh
        lsusb # Again, look for STMicroelectronics
        sudo chmod 666 /dev/bus/usb/001/002  # Replace 001/002 with numbers from lsusb (bus/device)
        ```

3. **Build & Flash**:
    ```sh
    west build -b nucleo_g431rb -- -DDTC_OVERLAY_FILE="../boards/nucleo_g431rb.overlay"
    west flash --runner openocd
    ```

4. **Monitor logs**
    ```sh
    sudo apt-get install minicom
    sudo minicom -s # Go to 'Serial port setup' and set 'A - Serial Device' to /dev/ttyACM0
    minicom
    ```
