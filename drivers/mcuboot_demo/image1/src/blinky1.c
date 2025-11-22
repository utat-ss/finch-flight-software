/* Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors*/
/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/dfu/mcuboot.h>
#include <zephyr/sys/reboot.h>

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED_NODE, gpios, {0});

void main(void)
{
	// 1. Initialization
	// If this image just booted successfully, mark it as "Good" so we don't revert.
	if (!boot_is_img_confirmed()) {
		printk("Confirming image...\n");
		boot_write_img_confirmed();
	}

	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

	printk("Running FAST blinky (Image 1).\n");
	printk("System will auto-swap in approx 10 seconds...\n");

	int blink_count = 0;
	int trigger_threshold = 50; // 50 * 200ms = 10 seconds

	// 2. The Main Loop
	while (1) {
		gpio_pin_toggle_dt(&led);
		k_msleep(200); // Fast blink

		blink_count++;

		// 3. The Automatic Trigger Check
		if (blink_count == trigger_threshold) {
			printk("Time's up! Requesting upgrade to slow blinky...\n");

			// A. Request the swap (Test Mode)
			// 'BOOT_UPGRADE_TEST' ensures that if the new image crashes,
			// the bootloader will swap BACK to this one automatically.
			int ret = boot_request_upgrade(BOOT_UPGRADE_TEST);

			if (ret == 0) {
				printk("Upgrade requested successfully. Rebooting now!\n");
				// Give the serial port a moment to flush the text
				k_msleep(100);

				// B. Software Reset (Simulates the Black Button)
				sys_reboot(SYS_REBOOT_COLD);
			} else {
				printk("Failed to request upgrade: %d\n", ret);
			}
		}
	}
}
