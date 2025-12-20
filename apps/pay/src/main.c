#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

/* Flash device node */
const struct device *flash_dev = DEVICE_DT_GET(DT_NODELABEL(flash10));

#define TEST_ADDRESS 0x000000
#define TEST_SIZE    256
uint8_t write_buf[TEST_SIZE];
uint8_t read_buf[TEST_SIZE] = {0};

int main(void)
{
    int ret;

    if (!device_is_ready(flash_dev)) {
        LOG_ERR("Device not ready");
        return 0;
    }

    LOG_INF("SPI Flash ready - starting test");

    ret = flash_erase(flash_dev, TEST_ADDRESS, 4096);
    if (ret < 0) {
        LOG_ERR("Erase failed: %d", ret);
        return 0;
    }

    
    for (int i = 0; i < TEST_SIZE; i += 4) {
        write_buf[i+0] = 0xDE;
        write_buf[i+1] = 0xAD;
        write_buf[i+2] = 0xBE;
        write_buf[i+3] = 0xEF;
    }

    ret = flash_write(flash_dev, TEST_ADDRESS, write_buf, TEST_SIZE);
    if (ret < 0) {
        LOG_ERR("Write failed: %d", ret);
        return 0;
    }

    ret = flash_read(flash_dev, TEST_ADDRESS, read_buf, TEST_SIZE);
    if (ret < 0) {
        LOG_ERR("Read failed: %d", ret);
        return 0;
    }

    for (int i = 0; i < TEST_SIZE; i++) {
        LOG_INF("%d %d", read_buf[i], write_buf[i]);
    }

    return 0;
}
