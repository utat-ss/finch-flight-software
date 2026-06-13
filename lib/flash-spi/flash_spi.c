#include "flash_spi.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(flash_spi, LOG_LEVEL_INF);

#define FLASH_PAGE_SIZE 256
#define FLASH_CMD_WRENB 0x06
#define FLASH_CMD_RDSR1 0x05
#define FLASH_CMD_CLPEF 0x30
#define FLASH_CMD_ER256_4B 0xDC
#define FLASH_CMD_PRPGE_4B 0x12
#define FLASH_CMD_READ_4B 0x13
#define FLASH_POLL_TIMEOUT_MS 10000

static const struct spi_config spi_cfg = {
    .frequency = 1875000,
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .slave = 0,
};

static int flash_xfer(const struct device *spi, const struct device *gpio, uint32_t cs_pin, const uint8_t *tx, uint8_t *rx, size_t len) {
    struct spi_buf tx_buf = { .buf = (void *)tx, .len = len };
    struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
    struct spi_buf rx_buf = { .buf = rx, .len = len };
    struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };

    gpio_pin_set(gpio, cs_pin, 1);
    gpio_pin_set(gpio, cs_pin, 0);

    int ret;
    if (rx != NULL) {
        ret = spi_transceive(spi, &spi_cfg, &tx_set, &rx_set);
    } else {
        ret = spi_write(spi, &spi_cfg, &tx_set);
    }

    int cs_ret = gpio_pin_set(gpio, cs_pin, 1);
    return (ret == 0) ? cs_ret : ret;
}

static int flash_read_status(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint8_t *status) {
    uint8_t tx[2] = {FLASH_CMD_RDSR1, 0x00};
    uint8_t rx[2] = {0};
    int ret = flash_xfer(spi, gpio, cs_pin, tx, rx, sizeof(tx));
    *status = rx[1];
    return ret;
}

static int flash_wait_ready(const struct device *spi, const struct device *gpio, uint32_t cs_pin) {
    /* We multiply the timeout by 100 because we are checking much faster now */
    for (int elapsed = 0; elapsed < (FLASH_POLL_TIMEOUT_MS * 100); elapsed++) {
        uint8_t status = 0;
        int ret = flash_read_status(spi, gpio, cs_pin, &status);
        if (ret != 0) return ret;

        if ((status & (BIT(6) | BIT(5))) != 0) {
            uint8_t clear_flags = FLASH_CMD_CLPEF;
            flash_xfer(spi, gpio, cs_pin, &clear_flags, NULL, 1);
            return -EIO;
        }
        if ((status & BIT(0)) == 0U) return 0;
        
        /* Spin the CPU for 10 microseconds instead of putting the RTOS to sleep */
        k_busy_wait(10); 
    }
    return -ETIMEDOUT;
}

int spi_flash_erase_sector(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint32_t addr) {
    const uint8_t wren = FLASH_CMD_WRENB;
    uint8_t erase_cmd[5] = { FLASH_CMD_ER256_4B, (addr >> 24), (addr >> 16), (addr >> 8), addr };

    /* 1. Send Write Enable */
    int ret = flash_xfer(spi, gpio, cs_pin, &wren, NULL, 1);
    if (ret != 0) return ret;

    /* 2. Send Erase Command */
    ret = flash_xfer(spi, gpio, cs_pin, erase_cmd, NULL, sizeof(erase_cmd));
    if (ret != 0) return ret;

    /* 3. Wait for the massive erase to finish (can take up to 1 second!) */
    return flash_wait_ready(spi, gpio, cs_pin);
}

int spi_flash_write(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint32_t addr, const uint8_t *data, size_t len) {
    if (len > FLASH_PAGE_SIZE) return -EINVAL;

    const uint8_t wren = FLASH_CMD_WRENB;

    /* 1. Send Write Enable (Required before every single 256-byte page) */
    int ret = flash_xfer(spi, gpio, cs_pin, &wren, NULL, 1);
    if (ret != 0) return ret;

    /* 2. Prepare ONLY the 5-byte command header on the stack */
    uint8_t header[5];
    header[0] = FLASH_CMD_PRPGE_4B;
    header[1] = (addr >> 24);
    header[2] = (addr >> 16);
    header[3] = (addr >> 8);
    header[4] = addr;

    /* 3. THE SCATTER-GATHER MAGIC 
     * We chain the header and the ROM data together. 
     * No memcpy! The CPU does zero work here.
     */
    struct spi_buf tx_bufs[] = {
        { .buf = header, .len = sizeof(header) },
        /* We cast to (void *) to satisfy the Zephyr struct, but it will not be modified */
        { .buf = (void *)data, .len = len } 
    };
    struct spi_buf_set tx_set = { .buffers = tx_bufs, .count = 2 };

    /* 4. Manually toggle CS and send the Buffer Set directly to the SPI/DMA driver */
    gpio_pin_set(gpio, cs_pin, 0);
    ret = spi_write(spi, &spi_cfg, &tx_set);
    gpio_pin_set(gpio, cs_pin, 1);

    if (ret != 0) return ret;

    /* 5. Wait for the chip to physically burn the data into silicon */
    return flash_wait_ready(spi, gpio, cs_pin);
}

int spi_flash_read(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint32_t addr, uint8_t *result, size_t len) {
    if (len > FLASH_PAGE_SIZE) return -EINVAL;

    uint8_t read_cmd[5 + FLASH_PAGE_SIZE] = {0}; 
    read_cmd[0] = FLASH_CMD_READ_4B;
    read_cmd[1] = (addr >> 24);
    read_cmd[2] = (addr >> 16);
    read_cmd[3] = (addr >> 8);
    read_cmd[4] = addr;
    
    uint8_t read_rx[5 + FLASH_PAGE_SIZE] = {0};
    int ret = flash_xfer(spi, gpio, cs_pin, read_cmd, read_rx, 5 + len);
    memcpy(result, &read_rx[5], len);
    return ret;
}