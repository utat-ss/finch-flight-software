#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);


int main(void) {
    uint32_t sectors;
    if (disk_access_ioctl("SD", DISK_IOCTL_GET_SECTOR_COUNT, &sectors) == 0) {
        printk("SD card sectors: %u\n", sectors);
    }
    uint8_t buf[512];
    disk_access_read("SD", buf, 0, 1);
    return 0;
}