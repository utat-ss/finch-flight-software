#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

//const struct device *adcs_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
const struct device *adcs_uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

void adcs_send_cmd(const uint8_t *cmd, size_t len){
    for(size_t i = 0; i < len; i++){
        uart_poll_out(adcs_uart, cmd[i]);
    }
    return;
}

int adcs_read_reply(uint8_t *response, size_t length){
    size_t i = 0;
    while(i < length){
        if (uart_poll_in(adcs_uart, &response[i]) == 0){
            i++;
            printk("true");
        }
        else{
            k_msleep(1);//wait for byte
            printk("false");
        }
    }
    return i;
}

int main (void){

    printk("hi");

    uint8_t cmd[] = { 0xC9, 0x01, 0x24, 0x00 };
    adcs_send_cmd(cmd, sizeof(cmd));//sends a command to the ADCS, requesting to read all telemetry data based upon the above header bytes

    uint8_t response[149];

    adcs_read_reply(response, sizeof(response));

    typedef struct {
    // 6‐byte timestamp 
    uint32_t time_s;
    uint16_t time_subsec;
    // magnetometer 
    float mag_x;
    float mag_y;
    float mag_z;
    // sun‐sensor unit vector (if digital sun sensors provide normalized output)
    float sun_unit_x;
    float sun_unit_y;
    float sun_unit_z;
    // accelerometer m/s^2
    float accel_x;
    float accel_y;
    float accel_z;
    // gyroscope (rad/s)
    float gyro_x;
    float gyro_y;
    float gyro_z;
    // reaction wheel speeds (RPM) and torques (mN·m)
    float rw_speed_x;
    float rw_speed_y;
    float rw_speed_z;
    float rw_torque_x;
    float rw_torque_y;
    float rw_torque_z;
    // magnetorquer currents (A)
    float mtq_current_x;
    float mtq_current_y;
    float mtq_current_z;
    // temperatures ()
    float temp_board;
    float temp_rw_x;
    float temp_rw_y;
    float temp_rw_z;
    // attitude quaternion (q0, q1, q2, q3)
    float quat_w;
    float quat_x;
    float quat_y;
    float quat_z;
    // stored angular momentum (nms)
    float momentum_x;
    float momentum_y;
    float momentum_z;
    } adcs_telemetry_t;

    adcs_telemetry_t telemetry;
    memcpy(&telemetry, &response[4], sizeof(adcs_telemetry_t));

    printk("%lf", telemetry.mag_x);
    printk("hello");
    return 0;

}