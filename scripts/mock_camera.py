import uasyncio as asyncio
from machine import UART

uart = UART(1, 115200)
rx_buf = bytearray()
loop = asyncio.get_event_loop()

def on_uart_irq(u):
    while u.any():
        rx_buf.extend(u.read(1))
    loop.create_task(handle_uart_data())

async def handle_uart_data():
    global rx_buf
    if rx_buf:
        print("Received:", rx_buf.decode(), end="")
        rx_buf = bytearray()  # clear after printing

async def main():
    print("hello world!")
    uart.irq(handler=on_uart_irq, trigger=UART.IRQ_RXIDLE)
    while True:
        await asyncio.sleep(0.01)  # give up control of the event loop

loop.run_until_complete(main())
