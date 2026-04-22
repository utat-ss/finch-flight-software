#!/usr/bin/env python3
#
# Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0
#
# Send a CSP-over-CAN (CFP2 / libcsp v2.1) request to the OBC and print the
# JEDEC ID returned by its SPI flash.
#
# The OBC firmware (apps/obc/src/main.c) binds a CSP service on port 10 that,
# on receipt of a payload starting with 0x0000, reads the JEDEC RDID from the
# on-board SPI flash and replies with the 6 ID bytes over CSP.
#
# This script encodes a single-fragment CSP packet (src=2, dst=1, dport=10,
# payload=0x0000) into one extended-ID CAN frame and then reassembles the
# multi-fragment reply (begin + more/end) from the bus.

import argparse
import time

import can

# --- CFP2 wire format (libcsp v2.1, csp_if_can.h) ---------------------------
#
# 29-bit extended CAN identifier layout:
#
#   bits 28-27  priority   (2)
#   bits 26-13  destination (14)
#   bits 12-7   sender      (6)   -- low bits of the source address
#   bits 6-5    packet count (2)
#   bits 4-2    frame count  (3)
#   bit  1      begin flag
#   bit  0      end flag
#
# The first ("begin") CAN frame also carries a 4-byte CFP2 header in its
# payload, big-endian, encoded as a u32:
#
#   bits 31-18  source      (14)
#   bits 17-12  dest port   (6)
#   bits 11-6   source port (6)
#   bits 5-0    csp flags   (6)

PRIO_OFFSET, PRIO_MASK = 27, 0x3
DST_OFFSET, DST_MASK = 13, 0x3FFF
SENDER_OFFSET, SENDER_MASK = 7, 0x3F
SC_OFFSET, SC_MASK = 5, 0x3
FC_OFFSET, FC_MASK = 2, 0x7
BEGIN_OFFSET = 1
END_OFFSET = 0

SRC_OFFSET, SRC_MASK = 18, 0x3FFF
DPORT_OFFSET, DPORT_MASK = 12, 0x3F
SPORT_OFFSET, SPORT_MASK = 6, 0x3F
FLAGS_OFFSET, FLAGS_MASK = 0, 0x3F

CSP_PRIO_NORM = 2

DEFAULT_OBC_ADDR = 1
DEFAULT_PI_ADDR = 2
DEFAULT_PORT = 10
DEFAULT_CHANNEL = "can0"


def _build_can_id(prio, dst, sender, sc, fc, begin, end):
    return (
        ((prio & PRIO_MASK) << PRIO_OFFSET)
        | ((dst & DST_MASK) << DST_OFFSET)
        | ((sender & SENDER_MASK) << SENDER_OFFSET)
        | ((sc & SC_MASK) << SC_OFFSET)
        | ((fc & FC_MASK) << FC_OFFSET)
        | ((begin & 1) << BEGIN_OFFSET)
        | ((end & 1) << END_OFFSET)
    )


def _build_begin_header(src, dport, sport, flags=0):
    value = (
        ((src & SRC_MASK) << SRC_OFFSET)
        | ((dport & DPORT_MASK) << DPORT_OFFSET)
        | ((sport & SPORT_MASK) << SPORT_OFFSET)
        | ((flags & FLAGS_MASK) << FLAGS_OFFSET)
    )
    return value.to_bytes(4, "big")


def _parse_can_id(can_id):
    return {
        "prio":   (can_id >> PRIO_OFFSET) & PRIO_MASK,
        "dst":    (can_id >> DST_OFFSET) & DST_MASK,
        "sender": (can_id >> SENDER_OFFSET) & SENDER_MASK,
        "sc":     (can_id >> SC_OFFSET) & SC_MASK,
        "fc":     (can_id >> FC_OFFSET) & FC_MASK,
        "begin":  (can_id >> BEGIN_OFFSET) & 1,
        "end":    (can_id >> END_OFFSET) & 1,
    }


def _parse_begin_header(data4):
    value = int.from_bytes(data4[:4], "big")
    return {
        "src":   (value >> SRC_OFFSET) & SRC_MASK,
        "dport": (value >> DPORT_OFFSET) & DPORT_MASK,
        "sport": (value >> SPORT_OFFSET) & SPORT_MASK,
        "flags": (value >> FLAGS_OFFSET) & FLAGS_MASK,
    }


def send_csp_short(bus, src_addr, dst_addr, dport, sport, payload,
                   priority=CSP_PRIO_NORM):
    """Send a single-fragment CSP packet (payload must fit in 4 bytes)."""
    if len(payload) > 4:
        raise ValueError("send_csp_short supports payloads <=4 bytes")

    can_id = _build_can_id(
        prio=priority, dst=dst_addr, sender=src_addr,
        sc=0, fc=0, begin=1, end=1,
    )
    data = _build_begin_header(src=src_addr, dport=dport, sport=sport) + bytes(payload)

    msg = can.Message(arbitration_id=can_id, data=data, is_extended_id=True)
    bus.send(msg)
    return msg


def recv_csp(bus, our_addr, peer_addr, timeout=5.0):
    """Reassemble one CSP packet addressed to us from a specific peer."""
    deadline = time.monotonic() + timeout

    header = None
    payload = bytearray()
    begin_seen = False
    expected_fc = 0

    while True:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError("timed out waiting for CSP reply")

        msg = bus.recv(timeout=remaining)
        if msg is None:
            raise TimeoutError("timed out waiting for CSP reply")
        if not msg.is_extended_id:
            continue

        fields = _parse_can_id(msg.arbitration_id)
        if fields["dst"] != our_addr or fields["sender"] != (peer_addr & SENDER_MASK):
            continue

        if fields["begin"]:
            if len(msg.data) < 4:
                continue
            header = _parse_begin_header(msg.data)
            payload = bytearray(msg.data[4:])
            begin_seen = True
            expected_fc = 1
            if fields["end"]:
                return header, bytes(payload)
            continue

        if not begin_seen:
            continue
        if fields["fc"] != expected_fc:
            continue

        payload.extend(msg.data)
        expected_fc = (expected_fc + 1) & FC_MASK
        if fields["end"]:
            return header, bytes(payload)


def read_jedec_id(bus, pi_addr, obc_addr, port, timeout=5.0):
    sent = send_csp_short(
        bus,
        src_addr=pi_addr, dst_addr=obc_addr,
        dport=port, sport=port,
        payload=b"\x00\x00",
    )
    print(f"tx CSP [id=0x{sent.arbitration_id:08x}] "
          f"src={pi_addr} dst={obc_addr} dport={port} data=0x{sent.data.hex()}")

    header, jedec = recv_csp(bus, our_addr=pi_addr, peer_addr=obc_addr,
                             timeout=timeout)
    return header, jedec


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--channel", default=DEFAULT_CHANNEL,
                        help=f"SocketCAN interface (default: {DEFAULT_CHANNEL})")
    parser.add_argument("--pi-addr", type=int, default=DEFAULT_PI_ADDR,
                        help=f"CSP address of this host (default: {DEFAULT_PI_ADDR})")
    parser.add_argument("--obc-addr", type=int, default=DEFAULT_OBC_ADDR,
                        help=f"CSP address of the OBC (default: {DEFAULT_OBC_ADDR})")
    parser.add_argument("--port", type=int, default=DEFAULT_PORT,
                        help=f"CSP port for the JEDEC service (default: {DEFAULT_PORT})")
    parser.add_argument("--timeout", type=float, default=5.0)
    args = parser.parse_args()

    bus = None
    try:
        bus = can.interface.Bus(interface="socketcan", channel=args.channel)

        header, jedec = read_jedec_id(
            bus,
            pi_addr=args.pi_addr,
            obc_addr=args.obc_addr,
            port=args.port,
            timeout=args.timeout,
        )

        pretty_id = " ".join(f"{b:02X}" for b in jedec)
        print(f"rx CSP src={header['src']} dport={header['dport']} "
              f"sport={header['sport']} flags=0x{header['flags']:02x}")
        print(f"JEDEC ID ({len(jedec)} bytes): {pretty_id}")

    except can.CanError as exc:
        print(f"CAN ERROR: {exc}")
        raise SystemExit(1)
    except TimeoutError as exc:
        print(f"TIMEOUT: {exc}")
        raise SystemExit(2)
    finally:
        if bus is not None:
            bus.shutdown()


if __name__ == "__main__":
    main()
