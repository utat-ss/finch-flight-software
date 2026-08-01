#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <ff.h>
#include <string.h>

LOG_MODULE_REGISTER(sd_test, LOG_LEVEL_INF);

/* FatFs work area */
static FATFS fat_fs;

/* Mount info */
static struct fs_mount_t mp = {
    .type = FS_FATFS,
    .fs_data = &fat_fs,
    .mnt_point = "/SD:",
};

int main(void)
{
    struct fs_file_t file;
    fs_file_t_init(&file);
    
    const char *file_path = "/SD:/test.txt";
    const char *test_str = "Nucleo H743ZI SD Card Test Successful!\n";
    char read_buffer[64] = {0};
    int rc;

    LOG_INF("Starting SD Card Test...");

    /* 1. Wait for disk to initialize */
    do {
        rc = disk_access_status("SD");
        if (rc == DISK_STATUS_OK) {
            break;
        }
        LOG_WRN("Disk not ready. Ensure SD card is inserted...");
        k_msleep(1000);
    } while (1);

    /* 2. Mount the filesystem */
    rc = fs_mount(&mp);
    if (rc != 0) {
        LOG_ERR("Error mounting disk. Ensure it is formatted to FAT32. Error: %d", rc);
        return rc;
    }
    LOG_INF("Disk mounted successfully at %s", mp.mnt_point);

    /* 3. Write to file */
    LOG_INF("Opening %s for writing...", file_path);
    rc = fs_open(&file, file_path, FS_O_CREATE | FS_O_WRITE);
    if (rc < 0) {
        LOG_ERR("Failed to open file for writing (err: %d)", rc);
        goto unmount;
    }

    rc = fs_write(&file, test_str, strlen(test_str));
    if (rc < 0) {
        LOG_ERR("Failed to write to file (err: %d)", rc);
    } else {
        LOG_INF("Wrote %d bytes to file.", rc);
    }
    
    fs_close(&file);

    /* 4. Read from file */
    LOG_INF("Opening %s for reading...", file_path);
    rc = fs_open(&file, file_path, FS_O_READ);
    if (rc < 0) {
        LOG_ERR("Failed to open file for reading (err: %d)", rc);
        goto unmount;
    }

    rc = fs_read(&file, read_buffer, sizeof(read_buffer) - 1);
    if (rc < 0) {
        LOG_ERR("Failed to read from file (err: %d)", rc);
    } else {
        LOG_INF("Read %d bytes: %s", rc, read_buffer);
    }

    fs_close(&file);

unmount:
    /* 5. Unmount */
    rc = fs_unmount(&mp);
    if (rc == 0) {
        LOG_INF("Disk unmounted successfully.");
    } else {
        LOG_ERR("Failed to unmount disk (err: %d)", rc);
    }

    return 0;
}