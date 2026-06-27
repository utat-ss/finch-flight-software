# Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

import can

BUS_CHANNEL = "can0"
BITRATE = 500000

bus = can.interface.Bus(
    interface="socketcan",
    channel=BUS_CHANNEL,
)

# Send ADCS ID request command
request = can.Message(
    arbitration_id=0x10,
    data=[0x01],
    is_extended_id=False,
)

try:
    bus.send(request)
    print(f"TX: ID=0x{request.arbitration_id:X} DATA={[hex(x) for x in request.data]}")
except can.CanError as e:
    print(f"Send failed: {e}")
    exit(1)

print("Listening for responses...")

while True:
    msg = bus.recv(timeout=1.0)

    if msg is None:
        continue

    if msg.is_error_frame:
       print(f"ERROR FRAME: {msg}")
       continue

    print(
        f"RX: ID=0x{msg.arbitration_id:X} "
        f"DLC={msg.dlc} "
        f"DATA={[hex(x) for x in msg.data]}"
    )
