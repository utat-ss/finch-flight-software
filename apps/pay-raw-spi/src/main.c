#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <string.h> 
#include <errno.h>

/* Include your global team library! */
#include "flash_spi.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* SPI controller and CS Pin */
#define SPI_NODE DT_NODELABEL(spi1)
#define CS_GPIO_NODE DT_NODELABEL(gpioe)
const char *gpio_set = "gpioe"; 

#define CS_PIN_0 6
#define WAIT_MS 1000
#define FLASH_TEST_ADDR 0x00040000U

static const struct spi_config spi_cfg = {
    .frequency = 20000000, /* 20 MHz */
    .operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB, 
    .slave = 0, 
};

int print_id(const struct device *spi, const struct device *gpio) {
    LOG_INF("SPI JEDEC test (manual CS)");
    LOG_INF("Configuring the CS Pin: %s-%d", gpio_set, CS_PIN_0);

    uint8_t cmd = 0x9F; /* JEDEC READ ID command */
    uint8_t rx[6] = {0};

    struct spi_buf tx_buf = { .buf = &cmd, .len = 1 };
    struct spi_buf rx_buf = { .buf = rx, .len = sizeof(rx) };
    struct spi_buf_set tx = { .buffers = &tx_buf, .count = 1 };
    struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };

    LOG_INF("Pin set to 0, waiting %d ms before spi_write", WAIT_MS);
    k_msleep(WAIT_MS);

    /* Assert CS manually for the ID check */
    gpio_pin_set(gpio, CS_PIN_0, 0);

    LOG_INF("Doing spi_write");
    int ret = spi_write(spi, &spi_cfg, &tx);
    if (ret != 0) {
        LOG_ERR("ERROR writing to SPI (ret=%d)", ret);
        gpio_pin_set(gpio, CS_PIN_0, 1);
        return ret;
    }

    LOG_INF("SPI write successful, waiting %d ms before read", WAIT_MS);
    k_msleep(WAIT_MS);

    /* Read response */
    ret = spi_read(spi, &spi_cfg, &rx_set);
    if (ret != 0) {
        LOG_ERR("ERROR reading from SPI (ret=%d)", ret);
        gpio_pin_set(gpio, CS_PIN_0, 1);
        return ret;
    }

    /* Deassert CS manually now that we have the ID */
    gpio_pin_set(gpio, CS_PIN_0, 1);

    LOG_INF("JEDEC ID: %02X %02X %02X %02X %02X %02X", rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
    return 0;
}

int benchmark_1mb_burst_write(const struct device *spi, const struct device *gpio) {
    LOG_INF("======================================");
    LOG_INF("Starting High-Speed 1 MB Benchmark...");

    /* 1 MB = 1,048,576 bytes. */
    const int chunk_size = 256;
    const int total_pages = 4096;
    const int sector_size = 65536; /* 64 KB per sector */
    const int total_sectors = 16;  /* 16 sectors * 64 KB = 1 MB */

    /* Create the dummy data dynamically in RAM */
    uint8_t dummy_page[256];
    memset(dummy_page, 0xAA, sizeof(dummy_page));

    /* --- STAGE 1: THE MASSIVE ERASE --- */
    LOG_INF("Stage 1: Pre-erasing 1 MB of memory (16 Sectors)...");
    uint32_t erase_start = k_uptime_get_32();
    
    for (int s = 0; s < total_sectors; s++) {
        uint32_t sector_addr = FLASH_TEST_ADDR + (s * sector_size);
        int ret = spi_flash_erase_sector(spi, gpio, CS_PIN_0, sector_addr);
        if (ret != 0) {
            LOG_ERR("Erase failed at sector %d", s);
            return ret;
        }
    }
    uint32_t erase_time = k_uptime_get_32() - erase_start;
    LOG_INF("Erase complete. Took %u ms.", erase_time);

    /* --- STAGE 2: THE BURST WRITE --- */
    LOG_INF("Stage 2: Burst writing 4,096 pages...");
    uint32_t write_start = k_uptime_get_32();

    for (int i = 0; i < total_pages; i++) {
        uint32_t current_addr = FLASH_TEST_ADDR + (i * chunk_size);
        int ret = spi_flash_write(spi, gpio, CS_PIN_0, current_addr, dummy_page, chunk_size);
        if (ret != 0) {
            LOG_ERR("Write failed at page %d", i);
            return ret;
        }
    }
    uint32_t write_time = k_uptime_get_32() - write_start;

    /* --- STAGE 3: THE VERIFY AND READ SPEED --- */
    LOG_INF("Stage 3: Reading and verifying 1 MB...");
    uint8_t read_buffer[256];
    uint32_t read_start = k_uptime_get_32();

    for (int i = 0; i < total_pages; i++) {
        uint32_t current_addr = FLASH_TEST_ADDR + (i * chunk_size);
        int ret = spi_flash_read(spi, gpio, CS_PIN_0, current_addr, read_buffer, chunk_size);
        if (ret != 0) {
            LOG_ERR("Read failed at page %d", i);
            return ret;
        }
        /* Verify data integrity */
        if (memcmp(dummy_page, read_buffer, chunk_size) != 0) {
            LOG_ERR("CRITICAL: Data corruption detected at page %d!", i);
            return -EIO;
        }
    }
    uint32_t read_time = k_uptime_get_32() - read_start;
    LOG_INF("Verification complete. 100%% Data Match.");
    
    /* Calculate actual speeds */
    float write_duration_sec = (float)write_time / 1000.0f;
    float write_kb_per_sec = (1024.0f) / write_duration_sec; 

    float read_duration_sec = (float)read_time / 1000.0f;
    float read_kb_per_sec = (1024.0f) / read_duration_sec; 

    LOG_INF("======================================");
    LOG_INF("1 MB BENCHMARK COMPLETE!");
    LOG_INF("Erase Time: %u ms", erase_time);
    LOG_INF("Write Time: %u ms (%.2f KB/s)", write_time, (double)write_kb_per_sec);
    LOG_INF("Read Time:  %u ms (%.2f KB/s)", read_time, (double)read_kb_per_sec);
    LOG_INF("======================================");

    return 0;
}

int main(void)
{
    const struct device *spi = DEVICE_DT_GET(SPI_NODE);
    const struct device *gpio = DEVICE_DT_GET(CS_GPIO_NODE);

    if (!device_is_ready(spi)) {
        LOG_ERR("SPI device not ready!");
        return -ENODEV;
    }

    if (!device_is_ready(gpio)) {
        LOG_ERR("GPIO device not ready!");
        return -ENODEV;
    }

    /* Configure CS pin to HIGH (Idle) */
    int ret = gpio_pin_configure(gpio, CS_PIN_0, GPIO_OUTPUT_HIGH);
    if (ret != 0) {
        LOG_ERR("ERROR configuring the CS Pin (ret=%d)", ret);
        return ret;
    }

    LOG_INF("Configured the CS Pin");

    /* 1. Run the JEDEC ID check */
    ret = print_id(spi, gpio);
    if (ret != 0) {
        LOG_ERR("ERROR reading JEDEC ID (ret=%d)", ret);
        return ret;
    }

    /* 2. Run the 1 MB Benchmark */
    ret = benchmark_1mb_burst_write(spi, gpio);
    if (ret != 0) {
        LOG_ERR("SPI Flash Benchmark failed (ret=%d)", ret);
    }

    return 0;
}