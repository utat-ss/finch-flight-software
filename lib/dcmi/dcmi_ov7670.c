#include <zephyr/kernel.h>
#include <zephyr/drivers/video.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include "dcmi/dcmi_ov7670.h"

LOG_MODULE_REGISTER(dcmi_ov7670);

int camera_init(const struct device* cam, struct video_format* fmt) {
    if (!device_is_ready(cam)) {
        LOG_ERR("Camera not ready");
        return -1;
    }
    if (video_set_format(cam, fmt)) {
        LOG_ERR("Failed to set camera format");
        return -1;
    }
    if (video_stream_start(cam, VIDEO_BUF_TYPE_OUTPUT)) {
        LOG_ERR("Failed to start streaming");
        return -1;
    }
    return 0;
}
    
void camera_loop(const struct device* cam) {
    while (1) {
        struct video_buffer *frame;
        if (!video_dequeue(cam, &frame, K_FOREVER)) {
            printk("Captured frame: size = %d\n", frame->size);
            video_enqueue(cam, frame);
        }
    }
}