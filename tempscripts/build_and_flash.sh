#!/bin/bash
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

echo "=== Building MCUboot ==="
west build -b nucleo_g431rb /bootloader/mcuboot/boot/zephyr -d /build/mcuboot --pristine -- \
  -DEXTRA_CONF_FILE=/tmp/mcuboot_extra.conf \
  -DEXTRA_DTC_OVERLAY_FILE=/workspace/tempscripts/mcuboot.overlay
if [ $? -ne 0 ]; then echo "MCUboot build failed"; exit 1; fi

echo "=== Building blinky_1 ==="
west build -b nucleo_g431rb /workspace/apps/blinky1 -d /build/blinky1 --pristine
if [ $? -ne 0 ]; then echo "blinky1 build failed"; exit 1; fi

echo "=== Building blinky_2 ==="
west build -b nucleo_g431rb /workspace/apps/blinky2 -d /build/blinky2 --pristine  
if [ $? -ne 0 ]; then echo "blinky2 build failed"; exit 1; fi

# After building blinky2, relocate signed bin to slot 1 address
echo "=== Relocating blinky2 to slot 1 ==="
/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objcopy \
  -I binary -O ihex \
  --change-addresses 0x08014000 \
  /build/blinky2/zephyr/zephyr.signed.bin \
  /build/blinky2/zephyr/zephyr.signed.slot1.hex

echo "=== Flashing all ==="
"$SCRIPT_DIR"/container_flash_custom.sh
