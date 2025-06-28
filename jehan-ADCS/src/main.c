#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

//const struct device *adcs_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
const struct device *adcs_uart = DEVICE_DT_GET(DT_NODELABEL(usart1));


void adcs_send_cmd(const uint8_t *cmd, size_t len){
    for(size_t i = 0; i < len; i++){
        uart_poll_out(adcs_uart, cmd[i]);
    }
    return;
}

int adcs_read_reply(uint8_t *response, size_t length){
    size_t i = 0;
    while(i < length){
        int data_transmit = uart_poll_in(adcs_uart, &response[i]);
        if (data_transmit == -1){
            k_msleep(10);
            printk("false");
        }
        else{
            i++;
            printk("true");
        }
    }
    return i;
}


int main (void){

    if (adcs_uart == NULL){
        printk("null");
    }else{
        printk("device success");
    }
    printk("");
    printk("hi");

    uint8_t cmd[] = { 0xC9, 0x01, 0x03, 0x00, 0x33 }; //Test command: reads SNID of the ADCS. these need to be upadted still to access actual telemetry data
    adcs_send_cmd(cmd, sizeof(cmd));//sends a command to the ADCS, requesting to read all telemetry data based upon the above header bytes

    uint8_t response[12];

    int length = adcs_read_reply(response, sizeof(response));

    //printing output (SNID):
    for (int i = 0; i < length; i++){
        printk("%c", response[i]);
    }

    return 0;

}