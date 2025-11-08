#include <csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/arch/csp_time.h>
#include <stdio.h>

#define TEST_PORT   10
#define TEST_ADDR   1

// Simple receiver task
static void csp_recv_task(void *param) {
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, TEST_PORT);
    csp_listen(sock, 5);

    printf("Receiver ready on port %d\n", TEST_PORT);

    while (1) {
        csp_conn_t *conn;
        csp_packet_t *packet;

        conn = csp_accept(sock, CSP_MAX_TIMEOUT);
        if (conn == NULL)
            continue;

        while ((packet = csp_read(conn, 50)) != NULL) {
            printf("Received packet: %s\n", (char *) packet->data);
            csp_buffer_free(packet);
        }

        csp_close(conn);
    }
}

// Simple sender test
static void csp_send_test(void) {
    csp_conn_t *conn;
    csp_packet_t *packet;

    printf("Sending packet test...\n");

    conn = csp_connect(CSP_PRIO_NORM, TEST_ADDR, TEST_PORT, 1000, CSP_O_NONE);
    if (conn == NULL) {
        printf("Connection failed\n");
        return;
    }

    packet = csp_buffer_get(100);
    if (packet == NULL) {
        printf("No packet buffer\n");
        csp_close(conn);
        return;
    }

    snprintf((char *) packet->data, 100, "Hello from CSP!");
    packet->length = strlen((char *) packet->data) + 1;

    if (!csp_send(conn, packet, 1000)) {
        printf("Send failed\n");
        csp_buffer_free(packet);
    }

    csp_close(conn);
}

// Entry point to initialize and run basic send/receive
void csp_test_run(void) {
    printf("Initializing CSP...\n");

    csp_conf_t conf = {
        .address = TEST_ADDR
    };

    csp_init(&conf);
    csp_route_start_task(0, 0);

    // Start receiver thread
    csp_thread_handle_t rx_handle;
    csp_thread_create(csp_recv_task, "csp_rx", 2048, NULL, 0, &rx_handle);

    // Give receiver a moment to start
    csp_sleep_ms(500);

    // Run send test
    csp_send_test();

    printf("CSP test complete.\n");
}