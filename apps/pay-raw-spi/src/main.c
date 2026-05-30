#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>

/* Register logging module */
LOG_MODULE_REGISTER(spi_jedec);

/* SPI controller */
#define SPI_NODE DT_NODELABEL(spi1)

/* Manual CS GPIO. Controlling CS pin manually. */
#define CS_GPIO_NODE DT_NODELABEL(gpioa)
const char *gpio_set = "gpioa"; /* Used in log messages */

#define CS_PIN 4
#define WAIT_MS 1000

#define FLASH_CMD_WRENB 0x06 // write enable, must be sent before any write/erase operation
#define FLASH_CMD_RDSR1 0x05 // read status register
#define FLASH_CMD_SE     0x20 // sector erase
#define FLASH_CMD_PP     0x02 // page program, writing data into flash mem
#define FLASH_CMD_READ   0x03 // read bytes from a given mem address
#define FLASH_CMD_ULBPR  0x98 // unblock all protections

#define FLASH_STATUS_BUSY BIT(0)
#define FLASH_TEST_ADDR 0x00200000U
#define FLASH_POLL_TIMEOUT_MS 10000

static const struct spi_config spi_cfg = {
    .frequency = 1875000, /* 1 MHz */
    .operation = SPI_WORD_SET(8) |
                 SPI_TRANSFER_MSB, /* MSB/LSB doesn't matter if sending 1 byte. Should be MSB still */
    .slave = 0, /* ignored when CS is manual */
};

static int flash_xfer(const struct device *spi,
                      const struct device *gpio,
                      const uint8_t *tx,
                      uint8_t *rx,
                      size_t len)
{
    uint8_t dummy_rx[len];

    struct spi_buf tx_buf = { .buf = (void *)tx, .len = len };
    struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
    struct spi_buf rx_buf = { .buf = (rx != NULL) ? rx : dummy_rx, .len = len };
    struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };

    int ret = gpio_pin_set(gpio, CS_PIN, 0);
    if (ret != 0) {
        return ret;
    }

    k_busy_wait(1);

    ret = spi_transceive(spi, &spi_cfg, &tx_set, &rx_set);

    k_busy_wait(1);

    int cs_ret = gpio_pin_set(gpio, CS_PIN, 1);
    if ((ret == 0) && (cs_ret != 0)) {
        ret = cs_ret;
    }

    k_busy_wait(1);

    return ret;
}


static int flash_read_status(const struct device *spi,
                             const struct device *gpio,
                             uint8_t *status)
{
    uint8_t tx[2] = {FLASH_CMD_RDSR1, 0x00};
    uint8_t rx[2] = {0};

    int ret = flash_xfer(spi, gpio, tx, rx, sizeof(tx));
    if (ret != 0) {
        return ret;
    }

    *status = rx[1];
    return 0;
}

static int flash_wait_ready(const struct device *spi, const struct device *gpio)
{
    k_msleep(10);

    // FLASH_POLL_TIMEOUT_MS
    for (int elapsed = 0; elapsed < FLASH_POLL_TIMEOUT_MS; elapsed++) {
        uint8_t status = 0;
        int ret = flash_read_status(spi, gpio, &status);
        if (ret != 0) {
            return ret;
        }

        if ((status & FLASH_STATUS_BUSY) == 0U) {
            return 0;
        }

        k_msleep(1);
    }

    LOG_ERR("Timed out waiting for flash ready");
    return -ETIMEDOUT;
}

static int flash_global_unlock(const struct device *spi, const struct device *gpio)
{
    uint8_t wren = FLASH_CMD_WRENB;
    uint8_t ulbpr = FLASH_CMD_ULBPR;

    int ret = flash_xfer(spi, gpio, &wren, NULL, 1);
    if (ret != 0) {
        return ret;
    }

    ret = flash_xfer(spi, gpio, &ulbpr, NULL, 1);
    if (ret != 0) {
        return ret;
    }

    return flash_wait_ready(spi, gpio);
}


int write(const struct device *spi, const struct device *gpio, const uint8_t *data) {
    const uint32_t addr = FLASH_TEST_ADDR;
    const uint8_t wren = FLASH_CMD_WRENB;

    
    // erasing target sector before writing
    uint8_t erase_cmd[4] = {
        FLASH_CMD_SE,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)addr,
    };
    int ret = flash_xfer(spi, gpio, &wren, NULL, 1);
    if (ret != 0) {
        LOG_ERR("WREN before erase failed (ret=%d)", ret);
        return ret;
    }
    ret = flash_xfer(spi, gpio, erase_cmd, NULL, sizeof(erase_cmd));
    if (ret != 0) {
        LOG_ERR("Sector erase command failed (ret=%d)", ret);
        return ret;
    }

    // waiting until flash chip ready for next command
    ret = flash_wait_ready(spi, gpio);
    if (ret != 0) {
        LOG_ERR("Sector erase did not complete (ret=%d)", ret);
        return ret;
    }

    // writing data to flash mem
    uint8_t program_cmd[8] = {
        FLASH_CMD_PP,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)addr,
        data[0],
        data[1],
        data[2],
        data[3],
    };

    ret = flash_xfer(spi, gpio, &wren, NULL, 1);
    if (ret != 0) {
        LOG_ERR("WREN before program failed (ret=%d)", ret);
        return ret;
    }

    ret = flash_xfer(spi, gpio, program_cmd, NULL, sizeof(program_cmd));
    if (ret != 0) {
        LOG_ERR("Page program command failed (ret=%d)", ret);
        return ret;
    }

    // waiting until flash chip ready for next command
    ret = flash_wait_ready(spi, gpio);

    if (ret != 0) {
        LOG_ERR("Page program did not complete (ret=%d)", ret);
        return ret;
    }

    return 0;
}

int read(const struct device *spi, const struct device *gpio, uint8_t *result) {
    const uint32_t addr = FLASH_TEST_ADDR;

    uint8_t read_cmd[8] = {
        FLASH_CMD_READ,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)addr,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    uint8_t read_rx[8] = {0};

    int ret = flash_xfer(spi, gpio, read_cmd, read_rx, sizeof(read_cmd));
    if (ret != 0) {
        LOG_ERR("Readback command failed (ret=%d)", ret);
        return ret;
    }

    result[0] = read_rx[4];
    result[1] = read_rx[5];
    result[2] = read_rx[6];
    result[3] = read_rx[7];
    LOG_INF("Read back: %02X %02X %02X %02X", result[0], result[1], result[2], result[3]);

    return 0;
}

int write_test(const struct device *spi, const struct device *gpio) {
    LOG_INF("SPI Flash write test");
    
    const uint8_t data[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    // unlock block protection here
    int ret = flash_global_unlock(spi, gpio);
    if (ret != 0) {
        LOG_ERR("Global unlock failed (ret=%d)", ret);
        return ret;
    }

    // writing

    ret = write(spi, gpio, data);
    if (ret != 0) {
        LOG_ERR("Write failed (ret=%d)", ret);
        return ret;
    }

    // reading
    uint8_t read_data[4] = {0};
    ret = read(spi, gpio, read_data);
    if (ret != 0) {
        LOG_ERR("Read failed (ret=%d)", ret);
        return ret;
    }

    for (int i = 0; i < 4; i++) {
        if (read_data[i] != data[i]) {
            LOG_ERR("Data mismatch at index %d: expected %02X, got %02X", i, data[i], read_data[i]);
            return -EIO;
        }
    }

    LOG_INF("Successfully programmed");
    return 0;
}

int print_id(const struct device *spi, const struct device *gpio) {

    LOG_INF("SPI JEDEC test (manual CS)");
    LOG_INF("Configuring the CS Pin: %s-%d", gpio_set, CS_PIN);

    uint8_t cmd = 0x9F; /* JEDEC READ ID command */
    uint8_t rx[6] = {0};

    struct spi_buf tx_buf = {
        .buf = &cmd,
        .len = 1,
    };

    struct spi_buf_set tx_set = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf rx_buf = {
        .buf = rx,
        .len = sizeof(rx),
    };

    struct spi_buf_set rx_set = {
        .buffers = &rx_buf,
        .count = 1,
    };

    // setting pin low (active)
    gpio_pin_set(gpio, CS_PIN, 0);
    LOG_INF("Pin set to 0, waiting %d ms before spi_transceive", WAIT_MS);
    k_msleep(WAIT_MS);

    // performing tranceive (sending command and reading response)
    int ret = spi_transceive(spi, &spi_cfg, &tx_set, &rx_set);
    if (ret != 0) {
        LOG_ERR("ERROR writing to/reading from SPI (ret=%d)", ret);
        return ret;
    }

    // setting pin high (inactive)
    gpio_pin_set(gpio, CS_PIN, 1);
    LOG_INF("Raw response: %02X %02X %02X %02X %02X %02X", rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
    LOG_INF("JEDEC ID: %02X %02X %02X", rx[1], rx[2], rx[3]);

    return 0;
}

int main(void)
{
    const struct device *spi = DEVICE_DT_GET(SPI_NODE);
    const struct device *gpio = DEVICE_DT_GET(CS_GPIO_NODE);

    LOG_INF("Starting main()--");

    // configuring CS pin as output and setting it high (inactive)
    int ret = gpio_pin_configure(gpio, CS_PIN, GPIO_OUTPUT_HIGH);
    if (ret != 0) {
        LOG_ERR("Failed to configure CS pin");
        return ret;
    }


    // getting the JDEC ID of the flash chip and printing it
    ret = print_id(spi, gpio);
    if (ret != 0) {
        LOG_ERR("SPI JEDEC test failed (ret=%d)", ret);
    }

    // write test: erasing a sector, programming 4 bytes, and reading back to verify
    
    ret = write_test(spi, gpio);
    if (ret != 0) {
        LOG_ERR("SPI flash write test failed (ret=%d)", ret);
    }

}