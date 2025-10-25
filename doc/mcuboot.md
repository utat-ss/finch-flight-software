# mcuboot
## build
`west build -p -b nucleo_g431rb /workspace/mcuboot/boot/zephyr/`

## flash
`/workspace/scripts/container_flash.sh`

# blinky
## build
`west build -p -b nucleo_g431rb /zephyr/samples/basic/blinky`

## Turn into elf
mcuboot produces a `.signed.bin`

`/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objcopy -I binary -O elf32-littlearm   --change-section-address .data=0x08008800   /build/zephyr/zephyr.signed.bin /build/zephyr/zephyr.elf`

address depends on overlay conf
`/zephyr/samples/subsys/nvs/boards/nucleo_g431rb.overlay`

Image 1: `0x08800`
Image 2: `0x14000`

# ## flash
`/workspace/scripts/container_flash.sh`

# blinky fast
Update version number in `blinky_fast/prj.conf` to a version higher than that of `blinky/prj.conf` since direct-xip chooses the latest image.

Do it again but build `/zephyr/samples/basic/blinky_fast`.
Then on objcopy, change the address to image 2 (`.data=0x08014000`)
And then flash normally

