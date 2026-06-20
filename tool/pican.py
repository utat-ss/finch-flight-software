# Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

import can
import time

BUS_CHANNEL = "can0"
BITRATE = 500000

bus = can.interface.Bus(
    interface="socketcan",
    channel=BUS_CHANNEL
)

print("Listening for OBC messages...")

last_tx = 0

while True:
    msg = bus.recv(timeout=1.0)

    if msg:
        print(
            f"RX: ID=0x{msg.arbitration_id:X} "
            f"DLC={msg.dlc} "
            f"DATA={[hex(x) for x in msg.data]}"
        )

        # If we receive the OBC message, send one back
        if msg.arbitration_id == 0x100:
            reply = can.Message(
                arbitration_id=0x100,
                data=[0xAA, 0xBB, 0xCC, 0xDD],
                is_extended_id=False
            )

            try:
                bus.send(reply)
                print("TX: Sent reply to OBC")
            except can.CanError as e:
                print(f"Send failed: {e}")
