#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(eps);

int main(void)
{
	while (1) {
		LOG_INF("eps");
		k_msleep(1000);
	}
	return 0;
}
