#!/usr/bin/env python3
# Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0
"""
Send a libcsp command to the OBC over SocketCAN and print the response.

This is a Python port of the libcsp ``simple-send-canbus`` C sample,
specialised for the FINCH OBC's command protocol. It uses libcsp's
official Python bindings (``libcsp_py3``) so the on-the-wire format is
guaranteed to match the OBC's libcsp stack.

Building the bindings (one-time, on the flatsat host where SocketCAN
lives). ``$LIBCSP`` is wherever the libcsp source tree was checked out
by ``west update`` — usually ``<workspace>/modules/lib/libcsp``:

    cmake -S "$LIBCSP" -B "$LIBCSP/build" \
          -DCSP_ENABLE_PYTHON3_BINDINGS=ON \
          -DCSP_HAVE_LIBSOCKETCAN=ON
    cmake --build "$LIBCSP/build"

Running:

    LD_LIBRARY_PATH="$LIBCSP/build" PYTHONPATH="$LIBCSP/build" \
        python3 scripts/flatsat/uart_sn_read.py adcs_snid_read
"""
import argparse
import struct
import sys
import time

import libcsp_py3 as libcsp

# ---- Must match include/finch/csp/csp.h ---------------------------------
FINCH_CSP_ADDR_OBC = 1
FINCH_CSP_OBC_PORT = 10
FINCH_CSP_CMD_ADCS_GET_ID = 0x0000

ADCS_ID_SIZE = 12

# Address this script claims for itself on the CSP network. Anything other
# than the OBC (1) or PAY (2) avoids collisions with real subsystems.
HOST_ADDR = 10

CSP_PRIO_NORM = 2

COMMAND_MAP = {
    # name           : (dport,              hex_payload,  reply_size)
    "adcs_snid_read": (FINCH_CSP_OBC_PORT,
                       f"{FINCH_CSP_CMD_ADCS_GET_ID:04x}",
                       ADCS_ID_SIZE),
}


def init_csp(channel: str, bitrate: int) -> None:
    """Bring up libcsp and the SocketCAN interface, mirroring
    ``csp_init`` + ``csp_can_socketcan_open_and_add_interface`` in
    ``simple-send-canbus``.
    """
    libcsp.init("flatsat", "finch-flight-software", "dev")

    # libcsp_py3 signature: can_socketcan_init(ifc, addr=0, bitrate=1000000,
    #                                          promisc=0)
    # Note ``promisc=1`` so we receive replies regardless of how the kernel
    # interprets our local address.
    libcsp.can_socketcan_init(channel, HOST_ADDR, bitrate, 1)

    libcsp.route_start_task()
    # Give the router thread a moment to come up before we start sending.
    time.sleep(0.2)


def run_command(name: str, timeout_ms: int = 1000) -> bytes:
    """Mirror of the C ``simple-send-canbus`` send/connect/close path,
    plus a read of the reply (the OBC firmware replies via
    ``csp_sendto_reply`` on the same connection).
    """
    dport, hex_payload, reply_size = COMMAND_MAP[name]
    out_payload = bytes.fromhex(hex_payload)

    conn = libcsp.connect(CSP_PRIO_NORM, FINCH_CSP_ADDR_OBC, dport,
                          timeout_ms, 0)
    if conn is None:
        raise RuntimeError("csp_connect() failed")

    try:
        packet = libcsp.buffer_get()
        if packet is None:
            raise RuntimeError("csp_buffer_get() returned NULL")
        libcsp.packet_set_data(packet, out_payload)
        libcsp.send(conn, packet, timeout_ms)
        print(f"tx -> dst={FINCH_CSP_ADDR_OBC} dport={dport} "
              f"data=0x{out_payload.hex()}")

        # The OBC replies on the same connection; read it back.
        reply = libcsp.read(conn, timeout_ms)
        if reply is None:
            raise RuntimeError("no response (timeout)")
        data = bytes(libcsp.packet_get_data(reply))
        length = libcsp.packet_get_length(reply)
        return data[:length]
    finally:
        libcsp.close(conn)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("command", choices=COMMAND_MAP.keys())
    parser.add_argument("--channel", default="can0",
                        help="SocketCAN channel (default: can0)")
    parser.add_argument("--bitrate", type=int, default=1_000_000,
                        help="CAN bitrate in bps (default: 1000000, must "
                             "match CONFIG_FINCH_CSP_CAN_BITRATE)")
    parser.add_argument("--timeout", type=int, default=1000,
                        help="Per-call timeout in milliseconds (default: 1000)")
    args = parser.parse_args()

    init_csp(args.channel, args.bitrate)

    # Sanity check the link with a CSP ping before issuing the command. A
    # negative value means the OBC didn't reply at all and there's no point
    # in sending the real request.
    rtt = libcsp.ping(FINCH_CSP_ADDR_OBC, args.timeout, 8, 0)
    print(f"ping OBC ({FINCH_CSP_ADDR_OBC}): {rtt} ms")
    if rtt < 0:
        print("OBC did not respond to ping; check bitrate, wiring, and "
              "CONFIG_FINCH_CSP_CAN_BITRATE on the OBC firmware.")
        sys.exit(1)

    try:
        reply = run_command(args.command, args.timeout)
    except RuntimeError as exc:
        print(f"error: {exc}")
        sys.exit(1)

    print(f"reply: 0x{reply.hex()}  ({len(reply)} byte"
          f"{'' if len(reply) == 1 else 's'})")


if __name__ == "__main__":
    main()
