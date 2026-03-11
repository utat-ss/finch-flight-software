/* Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors*/
/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/sys/reboot.h>

#define LED_NODE DT_ALIAS(led0)
#define SW0_NODE DT_ALIAS(sw0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED_NODE, gpios, {0});
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb;

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button pressed! Requesting upgrade to fast blinky...\n");

	int ret = boot_request_upgrade(BOOT_UPGRADE_TEST);
	if (ret) {
		printk("Failed to request upgrade: %d\n", ret);
		return;
	}

	printk("Upgrade requested, rebooting in next reset...\n");

	// could command a system reboot on button press or hold off until next reset

	// k_msleep(100);
	// sys_reboot(SYS_REBOOT_COLD);
}

void main(void)
{
	if (!boot_is_img_confirmed()) {
		printk("Confirming image...\n");
		boot_write_img_confirmed();
	}

	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

	gpio_pin_configure_dt(&button, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button_cb, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb);

	printk("Running slow blinky. Press button to swap.\n");

	while (1) {
		gpio_pin_toggle_dt(&led);
		k_msleep(1000); // Slow blink: 1 sec
	}
}
