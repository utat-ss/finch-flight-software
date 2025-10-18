/*
 * Single-board CAN test with loopback mode and LED toggling
 */

/*
 * CAN bus communication with TX and RX capability
 * For STM32G431RB Nucleo with FDCAN1
 */

 // Basically this entire code is AI generated, so some of the functions and techniques
 // used here I have no idea about

#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(can_main, LOG_LEVEL_DBG);

/* Get CAN device from device tree chosen node */
const struct device *can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

/* Buffer to store received CAN messages */
#define RX_BUFFER_SIZE 32
struct can_message_buffer {
    struct can_frame frame;
    k_timepoint_t timestamp;
    bool valid;
};

static struct can_message_buffer rx_buffer[RX_BUFFER_SIZE];
static uint32_t rx_write_idx = 0;
static uint32_t rx_read_idx = 0;

/* Mutex for thread-safe buffer access */
K_MUTEX_DEFINE(rx_buffer_mutex);

/* Semaphore to signal when new message arrives */
K_SEM_DEFINE(rx_sem, 0, RX_BUFFER_SIZE);

/**
 * RX Callback - Called when CAN message is received
 * This runs in ISR context, so keep it fast!
 */
void rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    /* Toggle LED on receive */
    gpio_pin_toggle_dt(&led);
    
    /* Store in circular buffer */
    k_mutex_lock(&rx_buffer_mutex, K_FOREVER);
    
    memcpy(&rx_buffer[rx_write_idx].frame, frame, sizeof(struct can_frame));
    rx_buffer[rx_write_idx].timestamp = sys_timepoint_calc(K_NO_WAIT);
    rx_buffer[rx_write_idx].valid = true;
    
    rx_write_idx = (rx_write_idx + 1) % RX_BUFFER_SIZE;
    
    /* If buffer full, overwrite oldest */
    if (rx_write_idx == rx_read_idx) {
        rx_read_idx = (rx_read_idx + 1) % RX_BUFFER_SIZE;
        LOG_WRN("RX buffer overflow, oldest message lost");
    }
    
    k_mutex_unlock(&rx_buffer_mutex);
    
    /* Signal that new message is available */
    k_sem_give(&rx_sem);
}

/**
 * TX Callback - Called when CAN message transmission completes
 */
void tx_callback(const struct device *dev, int error, void *user_data) {
    if (error) {
        LOG_ERR("CAN TX error: %d", error);
    } else {
        LOG_DBG("CAN frame sent successfully");
    }
}

/**
 * Process received CAN message
 * Add your application logic here
 */
void process_received_message(struct can_frame *frame) {
    LOG_INF("Processing RX: ID=0x%03X, DLC=%d", frame->id, frame->dlc);
    
    /* Print received data in hex */
    LOG_HEXDUMP_INF(frame->data, frame->dlc, "RX Data:");
    
    /* Example: Process based on CAN ID */
    switch (frame->id) {
        case 0x100:
            LOG_INF("Command message received");
            if (frame->dlc > 0) {
                switch (frame->data[0]) {
                    case 0x01:
                        LOG_INF("  -> Command: LED ON");
                        gpio_pin_set_dt(&led, 1);
                        break;
                    case 0x02:
                        LOG_INF("  -> Command: LED OFF");
                        gpio_pin_set_dt(&led, 0);
                        break;
                    case 0x03:
                        LOG_INF("  -> Command: LED TOGGLE");
                        gpio_pin_toggle_dt(&led);
                        break;
                    default:
                        LOG_INF("  -> Unknown command: 0x%02X", frame->data[0]);
                }
            }
            break;
            
        case 0x200:
            LOG_INF("Sensor data received");
            if (frame->dlc >= 4) {
                /* Example: Parse 32-bit sensor value from bytes 0-3 */
                uint32_t sensor_value = (frame->data[0] << 24) | 
                                       (frame->data[1] << 16) |
                                       (frame->data[2] << 8) | 
                                       frame->data[3];
                LOG_INF("  -> Sensor value: %u", sensor_value);
            }
            break;
            
        case 0x300:
            LOG_INF("Status message received");
            /* Add your status processing logic */
            break;
            
        default:
            LOG_INF("Unknown message ID: 0x%03X", frame->id);
            break;
    }
}

/**
 * RX Processing Thread
 * Continuously checks for and processes received messages
 */
void rx_thread_fn(void *arg1, void *arg2, void *arg3) {
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);
    
    LOG_INF("RX processing thread started");
    
    while (1) {
        /* Wait for new message (blocks until message available) */
        k_sem_take(&rx_sem, K_FOREVER);
        
        /* Get message from buffer */
        k_mutex_lock(&rx_buffer_mutex, K_FOREVER);
        
        if (rx_read_idx != rx_write_idx && rx_buffer[rx_read_idx].valid) {
            struct can_frame frame;
            memcpy(&frame, &rx_buffer[rx_read_idx].frame, sizeof(struct can_frame));
            rx_buffer[rx_read_idx].valid = false;
            rx_read_idx = (rx_read_idx + 1) % RX_BUFFER_SIZE;
            
            k_mutex_unlock(&rx_buffer_mutex);
            
            /* Process the message (outside mutex to avoid blocking) */
            process_received_message(&frame);
        } else {
            k_mutex_unlock(&rx_buffer_mutex);
        }
    }
}

/* Define RX thread */
K_THREAD_DEFINE(rx_thread, 2048, rx_thread_fn, NULL, NULL, NULL, 5, 0, 0);

/**
 * Send CAN message helper function
 */
int send_can_message(uint32_t id, uint8_t *data, uint8_t len) {
    struct can_frame frame = {
        .flags = 0,
        .id = id,
        .dlc = len
    };
    
    if (len > 8) {
        LOG_ERR("Data length %d exceeds max 8 bytes", len);
        return -EINVAL;
    }
    
    memcpy(frame.data, data, len);
    
    int ret = can_send(can_dev, &frame, K_MSEC(100), tx_callback, NULL);
    if (ret != 0) {
        LOG_ERR("Failed to send CAN message: %d", ret);
    } else {
        LOG_INF("Sent CAN ID=0x%03X, DLC=%d", id, len);
    }
    
    return ret;
}

int main(void) {
    int ret;
    
    LOG_INF("=== CAN Bus Test Starting ===");
    
    /* Check CAN device */
    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready");
        return -1;
    }
    LOG_INF("✓ CAN device ready");
    
    /* Check LED device */
    if (!device_is_ready(led.port)) {
        LOG_ERR("LED device not ready");
        return -1;
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    LOG_INF("✓ LED configured");
    
    /* Set CAN mode to NORMAL for actual bus communication */
    /* Use CAN_MODE_LOOPBACK for single-board testing */
    ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);
    if (ret != 0) {
        LOG_ERR("Failed to set CAN mode: %d", ret);
        return -1;
    }
    LOG_INF("✓ CAN mode set to NORMAL");
    
    /* Start CAN controller */
    ret = can_start(can_dev);
    if (ret != 0) {
        LOG_ERR("Failed to start CAN: %d", ret);
        return -1;
    }
    LOG_INF("✓ CAN controller started");
    
    /* Setup receive filters */
    /* Filter 1: Accept messages with ID 0x100 */
    struct can_filter filter1 = {
        .flags = CAN_FILTER_DATA,
        .id = 0x100,
        .mask = CAN_STD_ID_MASK  /* Exact match */
    };
    
    int filter_id1 = can_add_rx_filter(can_dev, rx_callback, NULL, &filter1);
    if (filter_id1 < 0) {
        LOG_ERR("Failed to add RX filter 1: %d", filter_id1);
        return -1;
    }
    LOG_INF("✓ RX filter added: ID=0x100 (filter_id=%d)", filter_id1);
    
    /* Filter 2: Accept messages with ID 0x200-0x2FF */
    struct can_filter filter2 = {
        .flags = CAN_FILTER_DATA,
        .id = 0x200,
        .mask = 0x700  /* Match ID bits 11-9 */
    };
    
    int filter_id2 = can_add_rx_filter(can_dev, rx_callback, NULL, &filter2);
    if (filter_id2 < 0) {
        LOG_ERR("Failed to add RX filter 2: %d", filter_id2);
    } else {
        LOG_INF("✓ RX filter added: ID=0x200-0x2FF (filter_id=%d)", filter_id2);
    }
    
    /* Filter 3: Accept messages with ID 0x123 (from your original code) */
    struct can_filter filter3 = {
        .flags = CAN_FILTER_DATA,
        .id = 0x123,
        .mask = CAN_STD_ID_MASK  /* Exact match */
    };
    
    int filter_id3 = can_add_rx_filter(can_dev, rx_callback, NULL, &filter3);
    if (filter_id3 < 0) {
        LOG_ERR("Failed to add RX filter 3: %d", filter_id3);
    } else {
        LOG_INF("✓ RX filter added: ID=0x123 (filter_id=%d)", filter_id3);
    }
    
    LOG_INF("=== CAN Bus Ready - Starting main loop ===\n");
    
    /* Main loop - send periodic messages */
    uint32_t counter = 0;
    
    while (1) {
        /* Example 1: Send counter value with ID 0x100 */
        uint8_t data1[6];
        data1[0] = 0xAA;  /* Header */
        data1[1] = (counter >> 24) & 0xFF;
        data1[2] = (counter >> 16) & 0xFF;
        data1[3] = (counter >> 8) & 0xFF;
        data1[4] = counter & 0xFF;
        data1[5] = 0x55;  /* Footer */
        
        send_can_message(0x100, data1, 6);
        
        k_msleep(500);
        
        /* Example 2: Send your original test message with ID 0x123 */
        uint8_t data2[3] = {0x01, 0x02, 0x03};
        send_can_message(0x123, data2, 3);
        
        counter++;
        k_msleep(500);
    }
    
    return 0;
}

/*
TX --> CAN_H (Green)
RX --> CAN_L (Yellow)

candump can0 goes into the satate where it'll receive can packets
ifconfig

yellow h
blue l

FDCAN1_RX, PA11, USB_DM - CAN Low
FDCAN1_TX, PA12, USB_DP - CAN High

PA12: CN10 12 (yellow)
PA11: CN10 14 (blue)

PB8 rx: CN10 3 (blue)
PB9 tx: CN10 5 (yellow)
*/