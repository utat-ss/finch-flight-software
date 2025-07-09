#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "PID.h"

// Simulated system variable
static int current_temperature = 20;  // Starting temperature

// Target temperature
#define TARGET_TEMP 75

// Simulate time in milliseconds
long getMillis() {
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    return spec.tv_sec * 1000 + spec.tv_nsec / 1e6;
}

// Feedback function
int read_temperature() {
    return current_temperature;
}

// Output function (heater control)
void apply_heater(int output) {
    // Simulate temperature change based on output
    // Cap the output to a reasonable effect for demonstration
    if (output > 100) output = 100;
    if (output < -100) output = -100;

    current_temperature += output * 0.01; // The 0.01 represents system response strength
    printf("Heater output: %d | Temp: %d\n", output, current_temperature);
}

int main() {
    // Create PID controller
    PIDController* pid = createPIDController(2.0, 0.1, 1.0, read_temperature, apply_heater);

    // Set target
    pid->target = TARGET_TEMP;

    // Register time function
    pid->timeFunctionRegistered = 1;
    pid->getSystemTime = getMillis;

    // Optional: set bounds
    setInputBounds(pid, 0, 100);
    setOutputBounds(pid, -100, 100);
    setMaxIntegralCumulation(pid, 1000);

    // Run the control loop for 60 cycles (~1 per 100ms)
    for (int i = 0; i < 60; i++) {
        tick(pid);
        struct timespec ts = {0, 100 * 1000000};  // Sleep 100ms
        nanosleep(&ts, NULL);
    }

    // Clean up
    free(pid);
    return 0;
}
