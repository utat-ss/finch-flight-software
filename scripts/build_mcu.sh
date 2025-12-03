#!/bin/bash

west build -p always -b nucleo_g431rb -d build_mcuboot /bootloader/mcuboot/boot/zephyr -- \
  -DCONF_FILE=/workspace/drivers/bootloader/prj.conf

# blinky1.c in mcuboot_demo CMake
west build -p always -b nucleo_g431rb -d build_fast drivers/mcuboot_demo/image1

# blinky2.c in mcuboot_demo CMake
west build -p always -b nucleo_g431rb -d build_slow drivers/mcuboot_demo/image2

# build own key for production - key used for signing images
# python3 /bootloader/mcuboot/scripts/imgtool.py keygen -k production-key-ec-p256.pem -t ecdsa-p256
