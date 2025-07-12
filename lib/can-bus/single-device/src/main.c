/*
 * Single-board CAN test with loopback mode and LED toggling
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(main);

const struct device *can_dev = DEVICE_DT_GET(DT_CHOSEN(fdcan1));
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    gpio_pin_toggle_dt(&led);
    LOG_INF("Received: 0x%02X", frame->data[0]);
}

int main(void)
{
    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready");
        return 0;
    }

    int ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);
    if (ret) {
        LOG_ERR("Failed to set loopback mode: %d", ret);
        return 0;
    }

    ret = can_set_bitrate(can_dev, 500000);  // 500kbps, standard CAN FD rate
    if (ret) {
        LOG_ERR("Failed to set bitrate: %d", ret);
        return 0;
    }

    ret = can_start(can_dev);
    if (ret) {
        LOG_ERR("Failed to start CAN controller: %d", ret);
        return 0;
    }

    if (!device_is_ready(led.port)) {
        LOG_ERR("LED device not ready");
        return 0;
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

    // setup receive filter
    struct can_filter filter = {
        .flags = 0U,
        .id = 0x100,
        .mask = CAN_STD_ID_MASK
    };
    can_add_rx_filter(can_dev, rx_callback, NULL, &filter);

    // message to send
    const struct can_frame frame = {
        .flags = 0,
        .id = 0x100,
        .dlc = 1,
        .data = {0x01}
    };

    while (1) {
        ret = can_send(can_dev, &frame, K_MSEC(100), NULL, NULL);

        if (ret == 0) {
            LOG_INF("Sent: 0x%02X", frame.data[0]);
        } else {
            LOG_ERR("Failed to send: %d", ret);
        }
        
        k_msleep(1000);
    }

    return 0;
}

/*
TX --> CAN_H (Green)
RX --> CAN_L (Yellow)
*/