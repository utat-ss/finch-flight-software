#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <zephyr/drivers/pinctrl.h>

//const struct device *adcs_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

const struct pinctrl_state* pinctrl_dev_config;

const struct device *adcs_uart = DEVICE_DT_GET(DT_NODELABEL(usart2));;

void adcs_send_cmd(const uint8_t *cmd, size_t len){
    for(size_t i = 0; i < len; i++){//added len -1 -- for(size_t i = 1; i < len; i++)
        uart_poll_out(adcs_uart, cmd[i]);
    }
    return;
}

int adcs_read_reply(uint8_t *response, size_t length){
    size_t i = 0;  
    //if (uart_poll_out(adcs_uart, &response[0]) == 0x00){ //checks if first byte being sent out is null- shouldn't mean anything- THIS LINE IS THE ERROR SENDING OUT THE ACCIDENTAL NULL BYTE
        //k_msleep(10);
    //}
    while(i < length){
        int data_transmit = uart_poll_in(adcs_uart, &response[i]);
        if (data_transmit == -1){
            k_msleep(10);
            printk("false");
        } else{
            i++;
            printk("%c", response[i]);
            printk("true");
        }
    }
    return i;
}


int main (void){
    
    k_msleep(50); //wait out any zephyr uart setup glitches

    //const struct pinctrl_dev_config *pcfg = PINCTRL_DT_DEV_CONFIG_GET(DT_NODELABEL(usart1));
    //pinctrl_apply_state(pcfg, PINCTRL_STATE_DEFAULT);

    

    if (device_is_ready(adcs_uart)){
        printk("device success");
    } else{
        printk("fail");
    }
    uint8_t cmd[] = { 0xC9, 0x01, 0x03, 0x00, 0x33 }; //Test command: reads SNID of the ADCS. these need to be upadted still to access actual telemetry data
    adcs_send_cmd(cmd, sizeof(cmd));//sends a command to the ADCS, requesting to read all telemetry data based upon the above header bytes

    //uint8_t response[12];

   // int length = adcs_read_reply(response, sizeof(response));

    //printing output (SNID):
    //for (int i = 0; i < length; i++){
        //printk("%c", response[i]);
    //}

    return 0;

}