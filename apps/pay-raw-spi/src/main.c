#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

/* Using the default spi1, gpioe configs from STM32. */
/* The pins used are: &spi1_sck_pa5 &spi1_miso_pa6 &spi1_mosi_pb5 */
/* CLK  = PA5. On the Nucleo board: CN7,pin 10 */
/* MISO = PA6.      On the Nucleo board: CN7,pin 12 */
/* MOSI = PA7.      On the Nucleo board: CN7,pin 14 */
/* Trying to communicate with 1 flash chip. CS (nCS) pin = PE4. On the Nucleo board: CN9,pin 16*/

/* Register logging module */
LOG_MODULE_REGISTER(spi_jedec);

/* SPI controller */
#define SPI_NODE DT_NODELABEL(spi1)

/* Manual CS GPIO. Controlling CS pin manually. */
#define CS_GPIO_NODE DT_NODELABEL(gpioe)
const char *gpio_set = "gpioe"; /* Used in log messages */

#define CS_PIN 4
#define WAIT_MS 1000

static const struct spi_config spi_cfg = {
    .frequency = 1000000, /* 1 MHz */
    .operation = SPI_WORD_SET(8) |
                 SPI_TRANSFER_MSB, /* MSB/LSB doesn't matter if sending 1 byte. Should be MSB still */
    .slave = 0, /* ignored when CS is manual */
};

int main(void)
{
    const struct device *spi = DEVICE_DT_GET(SPI_NODE);
    const struct device *gpio = DEVICE_DT_GET(CS_GPIO_NODE);

    uint8_t cmd = 0x9F; /* JEDEC READ ID command */
    uint8_t rx[3] = {0};

    struct spi_buf tx_buf = {
        .buf = &cmd,
        .len = 1,
    };

    struct spi_buf rx_buf = {
        .buf = rx,
        .len = sizeof(rx),
    };

    struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf_set rx_set = {
        .buffers = &rx_buf,
        .count = 1,
    };

    LOG_INF("SPI JEDEC test (manual CS)");
    LOG_INF("Configuring the CS Pin: %s-%d", gpio_set, CS_PIN);

    /* Configure CS pin */
    int ret = gpio_pin_configure(gpio, CS_PIN, GPIO_OUTPUT_HIGH);
    if (ret != 0) {
        LOG_ERR("ERROR configuring the CS Pin (ret=%d)", ret);
        return ret;
    }

    LOG_INF("Configured the CS Pin");
    LOG_INF("Setting the CS_PIN to low");

    /* Assert CS */
    ret = gpio_pin_set(gpio, CS_PIN, 0);
    if (ret != 0) {
        LOG_ERR("ERROR setting the CS pin low (ret=%d)", ret);
        return ret;
    }

    LOG_INF("Pin set to 0, waiting %d ms before spi_write", WAIT_MS);
    k_msleep(WAIT_MS);

    LOG_INF("Doing spi_write");

    /* Send command */
    ret = spi_write(spi, &spi_cfg, &tx);
    if (ret != 0) {
        LOG_ERR("ERROR writing to SPI (ret=%d)", ret);
        return ret;
    }

    LOG_INF("SPI write successful, waiting %d ms before read", WAIT_MS);
    k_msleep(WAIT_MS);

    /* Read response */
    ret = spi_read(spi, &spi_cfg, &rx_set);
    if (ret != 0) {
        LOG_ERR("ERROR reading from SPI (ret=%d)", ret);
        return ret;
    }

    LOG_INF("JEDEC ID: %02X %02X %02X", rx[0], rx[1], rx[2]);

    LOG_INF("Setting CS pin high in %d ms", WAIT_MS);
    k_msleep(WAIT_MS);

    /* Deassert CS */
    gpio_pin_set(gpio, CS_PIN, 1);

    LOG_INF("Sleeping forever");

    while (1) {
        k_sleep(K_FOREVER);
    }
}
