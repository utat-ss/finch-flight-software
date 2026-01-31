#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
 
LOG_MODULE_REGISTER(obc, LOG_LEVEL_DBG);
 
const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(usart1));
 
static void uart_cb(const struct device *dev, void *user_data)
{
    static int cb_count = 0;
    printk("Callback triggered %d\n", ++cb_count);
    while (uart_irq_update(dev) && uart_irq_is_pending(dev))
    {
        if (uart_irq_rx_ready(dev))
        { 
            uint8_t c;
            uart_poll_in(dev, &c); 
            printk("ISR received: 0x%02x\n", c);
        }
        else 
        {
            printk("No RX data in ISR\n");
        }
    }
}
 
int main(void)
{
    if (!device_is_ready(uart))
    {
        printk("UART device not found!\n");
        return 0;
    }
 
    struct uart_config config = {
        .baudrate = 115200, // Try 9600, 19200, etc. if needed
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    };
    if (uart_configure(uart, &config) != 0)
    {
        printk("UART configuration failed!\n");
        return 0;
    }
 
    int ret = uart_irq_callback_user_data_set(uart, uart_cb, NULL);
    printk("Callback set: %d\n", ret);
    uart_irq_rx_enable(uart);
 
    // Begin sending stuff here
 
    char bytes[] = {0xC9, 0x04, 0x01, 0x00, 0x32};
    for (int i = 0; i < sizeof(bytes); i++)
    {
        uart_poll_out(uart, bytes[i]); // send one byte at a time
        printk("Sent: 0x%02x\n", bytes[i]);
        k_msleep(10); // Small delay between bytes
    }
 
    printk("UART RX enabled, waiting for bytes...\n");
 
    unsigned char readBytes[9];
    int bytesRead = 0;
    while (bytesRead < 9)
    {
        uint8_t c;
        if (uart_poll_in(uart, &c) == 0)
        {
            printk("Polled: 0x%02x\n", c);
            readBytes[bytesRead] = c;
            bytesRead++;
            k_sleep(K_MSEC(10)); // Faster polling
        }else{
            k_sleep(K_MSEC(10)); // Faster polling
            printk("error is: %d\n", uart_poll_in(uart, &c));
            bytesRead++;
        }
    }
 
 
    // Change the mode (last 3 bits of the read body)
    readBytes[7] = (readBytes[7] & 0b11111000); // Set last 3 bits to 0 for safe mode
    readBytes[7] = (readBytes[7] | 0b00000001); // Set last 3 bits to 001 for detumbling mode
 
    char newByteMessage[] = {0xC8, 0x04, 0x01, 0x0, readBytes[4], readBytes[5], readBytes[6], readBytes[7], 0x00}; // Last byte is checksum and needs to be calculated
    char checkSum = 0x00;
    for (int i = 0; i < sizeof(newByteMessage) - 1; i++)
    {
        checkSum += newByteMessage[i];
    }
    checkSum = 0xff - checkSum + 1;
 
    newByteMessage[8] = checkSum;
 
    for (int i = 0; i < sizeof(newByteMessage); i++)
    {
        uart_poll_out(uart, newByteMessage[i]); // send one byte at a time
        printk("Sent: 0x%02x\n", newByteMessage[i]);
        k_msleep(10); // Small delay between bytes
    }
 
    // Now check current mode to make sure it was changed
 
    // use same 'bytes' array as before
    for (int i = 0; i < sizeof(bytes); i++)
    {
        uart_poll_out(uart, bytes[i]); // send one byte at a time
        printk("Sent: 0x%02x\n", bytes[i]);
        k_msleep(10); // Small delay between bytes
    }
 
    while (1)
    {
        uint8_t c;
        if (uart_poll_in(uart, &c) == 0)
        {
            printk("Received after mode change: Polled: 0x%02x\n", c);
        }
        k_sleep(K_MSEC(10)); // Faster polling
    }
}
 
/*
 
 
bool changeMode(int newMode[3]) { // 3 numbers 0 or 1
    //send command to read register
    char bytes[] = { 0xC9, 0x04, 0x01, 0x00, 0x32 };
    for (int i = 0; i < sizeof(bytes); i++) {
        uart_poll_out(uart, bytes[i]); //send one byte at a time
        k_msleep(10); // Small delay between bytes
    }
 
    //read output
    unsigned char readBytes[4];
    int bytesRead = 0;
    while (bytesRead < 4) {
        uint8_t c;
        if (uart_poll_in(uart, &c) == 0) {
            printk("Polled: 0x%02x\n", c);
            readBytes[bytesRead] = c;
            bytesRead++;
        }
        k_sleep(K_MSEC(10)); // Faster polling
    }
 
}
 
*/