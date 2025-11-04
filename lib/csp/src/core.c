/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>

#include <csp/csp.h>
#include <csp/interfaces/csp_if_lo.h>
#include <finch/csp/csp.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#if IS_ENABLED(CONFIG_FINCH_CSP_HAVE_CAN)
#include <csp/drivers/can_zephyr.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#endif

LOG_MODULE_REGISTER(finch_csp);

K_THREAD_STACK_DEFINE(finch_csp_router_stack, CONFIG_FINCH_CSP_ROUTER_STACK_SIZE);
static struct k_thread finch_csp_router_thread_data;
static k_tid_t finch_csp_router_tid;
static K_MUTEX_DEFINE(finch_csp_lock);
static bool finch_csp_initialized;

#if IS_ENABLED(CONFIG_FINCH_CSP_HAVE_CAN)
static const struct device *const finch_csp_can_dev =
	DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
static csp_iface_t *finch_csp_can_iface;
#endif

static void finch_csp_router_thread(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while (1) {
		(void)csp_route_work();
	}
}

static int finch_csp_bind_service(uint8_t port)
{
	int ret = csp_bind_callback(csp_service_handler, port);

	if ((ret == CSP_ERR_NONE) || (ret == CSP_ERR_USED) ||
	    (ret == CSP_ERR_ALREADY)) {
		return 0;
	}

	LOG_ERR("Failed to bind CSP service port %u (%d)", port, ret);
	return -EIO;
}

static int finch_csp_bind_services(void)
{
	static const uint8_t service_ports[] = {
		CSP_CMP,
		CSP_PING,
		CSP_MEMFREE,
		CSP_REBOOT,
		CSP_BUF_FREE,
		CSP_UPTIME,
	};

	for (size_t i = 0; i < ARRAY_SIZE(service_ports); i++) {
		int ret = finch_csp_bind_service(service_ports[i]);

		if (ret < 0) {
			return ret;
		}
	}

	return 0;
}

#if IS_ENABLED(CONFIG_FINCH_CSP_HAVE_CAN)
static int finch_csp_init_can(void)
{
	if (!device_is_ready(finch_csp_can_dev)) {
		LOG_ERR("CAN device %s not ready", finch_csp_can_dev->name);
		return -ENODEV;
	}

	int ret = csp_can_open_and_add_interface(finch_csp_can_dev,
						 CONFIG_FINCH_CSP_CAN_IFNAME,
						 CONFIG_FINCH_CSP_NODE_ADDRESS,
						 CONFIG_FINCH_CSP_CAN_BITRATE,
						 CONFIG_FINCH_CSP_NODE_ADDRESS,
						 CONFIG_FINCH_CSP_CAN_FILTER_MASK,
						 &finch_csp_can_iface);

	if (ret != CSP_ERR_NONE) {
		LOG_ERR("Failed to open CSP CAN interface (%d)", ret);
		return -EIO;
	}

	finch_csp_can_iface->is_default = 1;

	LOG_INF("CSP CAN interface up: %s (addr=%u, bitrate=%u)",
		finch_csp_can_iface->name,
		CONFIG_FINCH_CSP_NODE_ADDRESS,
		CONFIG_FINCH_CSP_CAN_BITRATE);

	return 0;
}
#endif /* CONFIG_FINCH_CSP_HAVE_CAN */

int finch_csp_init(void)
{
	int ret = 0;

	k_mutex_lock(&finch_csp_lock, K_FOREVER);

	if (finch_csp_initialized) {
		goto out;
	}

	csp_conf.hostname = CONFIG_FINCH_CSP_HOSTNAME;
	csp_conf.model = CONFIG_FINCH_CSP_MODEL;
	csp_conf.revision = CONFIG_FINCH_CSP_REVISION;

	csp_init();

	csp_if_lo.addr = CONFIG_FINCH_CSP_NODE_ADDRESS;

	ret = finch_csp_bind_services();
	if (ret < 0) {
		goto out;
	}

#if IS_ENABLED(CONFIG_FINCH_CSP_HAVE_CAN)
	ret = finch_csp_init_can();
	if (ret < 0) {
		goto out;
	}
#endif

	finch_csp_router_tid = k_thread_create(
		&finch_csp_router_thread_data,
		finch_csp_router_stack,
		K_THREAD_STACK_SIZEOF(finch_csp_router_stack),
		finch_csp_router_thread,
		NULL, NULL, NULL,
		CONFIG_FINCH_CSP_ROUTER_PRIORITY,
		0,
		K_NO_WAIT);

	if (finch_csp_router_tid == NULL) {
		LOG_ERR("Failed to creat CSP router thread");
		ret = -EIO;
		goto out;
	}

	k_thread_name_set(finch_csp_router_tid, "finch_csp_router");
	finch_csp_initialized = true;

	LOG_INF("CSP core ready (addr=%u, host=%s)",
		CONFIG_FINCH_CSP_NODE_ADDRESS,
		CONFIG_FINCH_CSP_HOSTNAME);

out:
	k_mutex_unlock(&finch_csp_lock);
	return ret;
}
