#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

/* Flash device node */
const struct device *flash_dev = DEVICE_DT_GET(DT_NODELABEL(flash1));

/* GPIO CS pins */
static const struct gpio_dt_spec cs0 = GPIO_DT_SPEC_GET(DT_NODELABEL(qspics0), gpios);
static const struct gpio_dt_spec cs1 = GPIO_DT_SPEC_GET(DT_NODELABEL(qspics1), gpios);

#define TEST_ADDRESS 0x000000
#define TEST_SIZE    256

int main(void)
{
    int ret;

    if (!device_is_ready(flash_dev)) {
        LOG_ERR("Device not ready");
        return 0;
    }

    /* Configure GPIO CS pins as output and set high initially */
    ret = gpio_pin_configure_dt(&cs0, GPIO_OUTPUT_HIGH);
    if (ret) return ret;
    ret = gpio_pin_configure_dt(&cs1, GPIO_OUTPUT_HIGH);
    if (ret) return ret;

    LOG_INF("QSPI Flash ready - starting test");

    /* Select flash chip 0 */
    gpio_pin_set_dt(&cs0, 0); // active low

    ret = flash_erase(flash_dev, TEST_ADDRESS, 4096);
    if (ret < 0) {
        LOG_ERR("Erase failed: %d", ret);
        gpio_pin_set_dt(&cs0, 1); // deselect
        return 0;
    }

    uint8_t write_buf[TEST_SIZE];
    for (int i = 0; i < TEST_SIZE; i += 4) {
        write_buf[i+0] = 0xDE;
        write_buf[i+1] = 0xAD;
        write_buf[i+2] = 0xBE;
        write_buf[i+3] = 0xEF;
    }

    ret = flash_write(flash_dev, TEST_ADDRESS, write_buf, TEST_SIZE);
    if (ret < 0) {
        LOG_ERR("Write failed: %d", ret);
        gpio_pin_set_dt(&cs0, 1); // deselect
        return 0;
    }

    uint8_t read_buf[TEST_SIZE] = {0};

    ret = flash_read(flash_dev, TEST_ADDRESS, read_buf, TEST_SIZE);
    if (ret < 0) {
        LOG_ERR("Read failed: %d", ret);
        gpio_pin_set_dt(&cs0, 1); // deselect
        return 0;
    }

    /* Deselect flash chip 0 */
    gpio_pin_set_dt(&cs0, 1);

    for (int i = 0; i < TEST_SIZE; i++) {
        LOG_INF("%d %d", read_buf[i], write_buf[i]);
    }

    return 0;
}
