#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/drivers/gpio.h>

/* Define the pins based on the DeviceTree labels we just created */
static const struct gpio_dt_spec trans_en = GPIO_DT_SPEC_GET(DT_NODELABEL(sd_trans_en), gpios);
static const struct gpio_dt_spec mux_en = GPIO_DT_SPEC_GET(DT_NODELABEL(sd_mux_en), gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);


int main(void) {

    /* 1. Initialize and enable the control pins */
    if (gpio_is_ready_dt(&trans_en)) {
        gpio_pin_configure_dt(&trans_en, GPIO_OUTPUT_ACTIVE);
    }
    if (gpio_is_ready_dt(&mux_en)) {
        gpio_pin_configure_dt(&mux_en, GPIO_OUTPUT_ACTIVE);
    }

    /* 2. Now perform the SD initialization */
    uint32_t sectors;
    if (disk_access_init("SD") == 0) {
        if (disk_access_ioctl("SD", DISK_IOCTL_GET_SECTOR_COUNT, &sectors) == 0) {
            printk("SD card sectors: %u\n", sectors);
        }
    } else {
        printk("SD initialization failed!\n");
    }

    uint8_t buf[512];
    disk_access_read("SD", buf, 0, 1);
    return 0;
}