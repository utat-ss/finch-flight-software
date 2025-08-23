/*
 * Single-board CAN test with loopback mode and LED toggling
 */

#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

const struct device *can_dev = DEVICE_DT_GET(DT_CHOSEN(fdcan1));
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

// void rx_callback(const struct device *dev, struct can_frame *frame, void
// *user_data) {
//     gpio_pin_toggle_dt(&led);
//     LOG_INF("Received: 0x%02X", frame->data[0]);
// }

void tx_callback(const struct device *dev, int error, void *user_data) {
  if (error) {
    printk("CAN TX error: %d\n", error);
  } else {
    printk("CAN frame sent successfully\n");
  }
}

int main(void) {
  if (!device_is_ready(can_dev)) {
    LOG_ERR("CAN device not ready");
    return 0;
  }
  LOG_INF("CAN device ready!!!!!!!!!");

  int ret;

  // ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);
  // if (ret) {
  //     LOG_ERR("Failed to set loopback mode: %d", ret);
  //     return 0;
  // }

  ret = can_set_bitrate(can_dev, 1000000);  // 500kbps, standard CAN FD rate
  if (ret) {
    LOG_ERR("Failed to set bitrate: %d", ret);
    return 0;
  }
  LOG_INF("Successfully set bitrate!!!!!!!");

  ret = can_start(can_dev);
  if (ret) {
    LOG_ERR("Failed to start CAN controller: %d", ret);
    return 0;
  }
  LOG_INF("Successfully started CAN controller!!!!!!!!");

  if (!device_is_ready(led.port)) {
    LOG_ERR("LED device not ready");
    return 0;
  }
  LOG_INF("LED device ready!!!!!!!!!!!");
  gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  LOG_INF("LED pin configured!!!!!!!!!!");

  // setup receive filter
  // struct can_filter filter = {
  //     .flags = 0U,
  //     .id = 0x100,
  //     .mask = CAN_STD_ID_MASK
  // };
  // can_add_rx_filter(can_dev, rx_callback, NULL, &filter);

  // message to send
  const struct can_frame frame = {.flags = 0,
                                  .id = 0x100,
                                  .dlc = 3,
                                  // .len = 3,
                                  .data = {0x01, 0x02, 0x03}};
  LOG_INF("Successfully constructed message!!!!!!!!!!");

  while (1) {
    LOG_INF("I AM IN THE LOOP!!!!!!!");
    ret = can_send(can_dev, &frame, K_MSEC(100), tx_callback, NULL);

    if (ret == 0) {
      LOG_INF("Sent:");
      for (int i = 0; i < 3; i++) {
        LOG_INF("\t 0x%02X", frame.data[i]);
      }
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