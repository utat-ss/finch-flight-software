#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(obc);

int main(void)
{
	while (1) {
		LOG_INF("obc");
		k_msleep(1000);
	}
	return 0;
}
