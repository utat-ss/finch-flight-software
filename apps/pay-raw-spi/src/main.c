#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>

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

#define CS_PIN 6
#define WAIT_MS 1000

#define FLASH_CMD_WRENB 0x06
#define FLASH_CMD_RDSR1 0x05
#define FLASH_CMD_CLPEF 0x30
#define FLASH_CMD_ER256_4B 0xDC
#define FLASH_CMD_PRPGE_4B 0x12
#define FLASH_CMD_READ_4B 0x13

#define FLASH_TEST_ADDR 0x00040000U
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
    struct spi_buf tx_buf = {
        .buf = (void *)tx,
        .len = len,
    };

    struct spi_buf_set tx_set = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf rx_buf = {
        .buf = rx,
        .len = len,
    };

    struct spi_buf_set rx_set = {
        .buffers = &rx_buf,
        .count = 1,
    };

    int ret = gpio_pin_set(gpio, CS_PIN, 1);
    if (ret != 0) {
        return ret;
    }

    ret = gpio_pin_set(gpio, CS_PIN, 0);
    if (ret != 0) {
        return ret;
    }

    if (rx != NULL) {
        ret = spi_transceive(spi, &spi_cfg, &tx_set, &rx_set);
    } else {
        ret = spi_write(spi, &spi_cfg, &tx_set);
    }

    int cs_ret = gpio_pin_set(gpio, CS_PIN, 1);
    if ((ret == 0) && (cs_ret != 0)) {
        ret = cs_ret;
    }

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
    for (int elapsed = 0; elapsed < FLASH_POLL_TIMEOUT_MS; elapsed++) {
        uint8_t status = 0;
        int ret = flash_read_status(spi, gpio, &status);
        if (ret != 0) {
            return ret;
        }

        if ((status & (BIT(6) | BIT(5))) != 0) {
            uint8_t clear_flags = FLASH_CMD_CLPEF;
            (void)flash_xfer(spi, gpio, &clear_flags, NULL, 1);
            LOG_ERR("Flash operation failed, status=0x%02X", status);
            return -EIO;
        }

        if ((status & BIT(0)) == 0U) {
            return 0;
        }

        k_msleep(1);
    }

    LOG_ERR("Timed out waiting for flash ready");
    return -ETIMEDOUT;
}

int write(const struct device *spi, const struct device *gpio, const uint8_t *data) {
    const uint32_t addr = FLASH_TEST_ADDR;
    const uint8_t wren = FLASH_CMD_WRENB;

    uint8_t erase_cmd[5] = {
        FLASH_CMD_ER256_4B,
        (uint8_t)(addr >> 24),
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

    ret = flash_wait_ready(spi, gpio);
    if (ret != 0) {
        LOG_ERR("Sector erase did not complete (ret=%d)", ret);
        return ret;
    }

    uint8_t program_cmd[9] = {
        FLASH_CMD_PRPGE_4B,
        (uint8_t)(addr >> 24),
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

    ret = flash_wait_ready(spi, gpio);
    if (ret != 0) {
        LOG_ERR("Page program did not complete (ret=%d)", ret);
        return ret;
    }

    return 0;
}

int read(const struct device *spi, const struct device *gpio, uint8_t *result) {
    const uint32_t addr = FLASH_TEST_ADDR;

    uint8_t read_cmd[9] = {
        FLASH_CMD_READ_4B,
        (uint8_t)(addr >> 24),
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)addr,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    uint8_t read_rx[9] = {0};

    int ret = flash_xfer(spi, gpio, read_cmd, read_rx, sizeof(read_cmd));
    if (ret != 0) {
        LOG_ERR("Readback command failed (ret=%d)", ret);
        return ret;
    }

    result[0] = read_rx[5];
    result[1] = read_rx[6];
    result[2] = read_rx[7];
    result[3] = read_rx[8];
    LOG_INF("Read back: %02X %02X %02X %02X", result[0], result[1], result[2], result[3]);

    return 0;
}

int write_test(const struct device *spi, const struct device *gpio) {
    LOG_INF("SPI Flash write test");
    
    const uint8_t data[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    //int ret = write(spi, gpio, data);
    //if (ret != 0) {
    //    LOG_ERR("Write failed (ret=%d)", ret);
    //    return ret;
    //}

    uint8_t read_data[4] = {0};
    int ret = read(spi, gpio, read_data);
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

    LOG_INF("Pin set to 0, waiting %d ms before spi_write", WAIT_MS);
    k_msleep(WAIT_MS);

    LOG_INF("Doing spi_write");

    /* Send command */
    int ret = spi_write(spi, &spi_cfg, &tx);
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

    LOG_INF("JEDEC ID: %02X %02X %02X %02X %02X %02X", rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
    return 0;
}

int main(void)
{
    const struct device *spi = DEVICE_DT_GET(SPI_NODE);
    const struct device *gpio = DEVICE_DT_GET(CS_GPIO_NODE);

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

    ret = print_id(spi, gpio);
    
    if (ret != 0) {
        LOG_ERR("ERROR configuring the CS Pin (ret=%d)", ret);
        return ret;
    }

    ret = write_test(spi, gpio);
    if (ret != 0) {
        LOG_ERR("SPI Flash write test failed (ret=%d)", ret);
    } else {
        LOG_INF("SPI Flash write test successful");
    }

    // Write
    // uint8_t data[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    // ret = write(spi, gpio, data);

    // if (ret != 0) {
    //     LOG_ERR("SPI Flash write failed (ret=%d)", ret);
    // } else {
    //     LOG_INF("SPI Flash write successful");
    // }

    // Read
    // uint8_t read_data[4] = {0};
    // ret = read(spi, gpio, read_data);
    // if (ret != 0) {
    //     LOG_ERR("SPI Flash read failed (ret=%d)", ret);
    // } else {
    //     LOG_INF("SPI Flash read successful: %02X %02X %02X %02X", read_data[0], read_data[1], read_data[2], read_data[3]);
    // }

    LOG_INF("Setting CS pin high in %d ms", WAIT_MS);
    k_msleep(WAIT_MS);

    /* Deassert CS */
    gpio_pin_set(gpio, CS_PIN, 1);
}
