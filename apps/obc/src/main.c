/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include <finch/adcs/adcs.h>
#include <finch/csp/csp.h>

#include <csp/csp.h>
#include <csp/csp_buffer.h>
#include <csp/csp_error.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);

static void obc_handle_adcs_request(csp_conn_t *conn, csp_packet_t *packet)
{
	if ((conn == NULL) || (packet == NULL)) {
		return;
	}

	uint8_t adcs_id[ADCS_ID_SIZE];
	csp_packet_t *reply = csp_buffer_get(ADCS_ID_SIZE);

	if (reply == NULL) {
		LOG_ERR("Failed to allocate CSP reply packet");
		csp_buffer_free(packet);
		return;
	}

	if (adcs_get_id(adcs_id) != ADCS_RET_OK) {
		LOG_ERR("adcs_get_id() failed");
		csp_buffer_free(reply);
		csp_buffer_free(packet);
		return;
	}

	memcpy(reply->data, adcs_id, ADCS_ID_SIZE);
	reply->length = ADCS_ID_SIZE;

	LOG_INF("CSP request src=%u:%u dst=%u:%u len=%u -> ADCS ID reply",
		packet->id.src, packet->id.sport,
		packet->id.dst, packet->id.dport,
		packet->length);

	csp_buffer_free(packet);
	csp_send(conn, reply);
}

int main(void)
{
	csp_socket_t sock = {0};

	int ret = finch_csp_init();

	if (ret < 0) {
		LOG_ERR("Failed to initialize FINCH CSP (%d)", ret);
		return ret;
	}

	ret = csp_bind(&sock, CSP_ANY);
	if (ret != CSP_ERR_NONE) {
		LOG_ERR("Failed to bind OBC CSP socket (%d)", ret);
		return -EIO;
	}

	ret = csp_listen(&sock, 5);
	if (ret != CSP_ERR_NONE) {
		LOG_ERR("Failed to listen on OBC CSP socket (%d)", ret);
		return -EIO;
	}

	LOG_INF("OBC ready, listening on CSP port %u", FINCH_CSP_OBC_PORT);

	while (1) {
		csp_conn_t *conn = csp_accept(&sock, CSP_MAX_TIMEOUT);

		if (conn == NULL) {
			continue;
		}

		while (1) {
			csp_packet_t *packet = csp_read(conn, 100);

			if (packet == NULL) {
				break;
			}

			if (csp_conn_dport(conn) != FINCH_CSP_OBC_PORT) {
				csp_service_handler(packet);
				continue;
			}

			obc_handle_adcs_request(conn, packet);
		}

		(void)csp_close(conn);
	}

	return 0;
}
