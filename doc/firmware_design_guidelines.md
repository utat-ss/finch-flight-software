*A living document of best practices for writing firmware.*

*This document is for firmware design guidelines. For coding style guidelines, see the [Zephyr Style Guide](https://docs.zephyrproject.org/latest/contribute/style/index.html).*

## Memory Allocation

The `malloc()` function should not be used and any dynamic memory allocation should be avoided after initialization. It is incredibly easy to mishandle memory and free routines, and they are unpredictable as to how much time they take to execute. Therefore, they are not safe for safety-critical applications.

All arrays must be statically declared using a fixed number of elements.

```c
int my_arr[3] = { 1, 2, 3 }; /* <--- Specify the size of 3 */
```

## Infinite Loops

Infinite loops must be avoided to ensure deterministic behavior and prevent the MCU from stalling. All loops must have a statically verifiable upper bound to guarantee termination. When waiting for hardware events (eg. pin state changes or sensor data), include a timeout mechanism to prevent indefinite blocking.

The following example demonstrates a loop that waits for the `PB2` pin to go low, with a timeout to ensure termination,

```c
/* UINT16_MAX is defined in <stdint.h> and evaluates to 2^16 - 1 */
uint16_t timeout = UINT16_MAX;
while (gpio_pin_get(gpio_eps, 2) && timeout > 0) {
    timeout--;
}

/* Log or handle timeout expiration */
if (timeout == 0) {
    /* Log error, reset peripheral or trigger fallback */
}
```

## Volatile Variables

Variables that are accessed across multiple threads, interface with hardware, or use a signal handler must be declared volatile to prevent compiler optimizations that could lead to incorrect behavior. The volatile qualifier ensures that the compiler does not cache the variable's value in registers, guaranteeing that each read or write accesses the actual memory location.

## Scope

Variables must be declared at the smallest possible scope to minimize unintended side effects and improve code maintainability. Avoid reusing variable names in nested scopes to prevent variable shadowing, which can introduce hard to debug errors.

```c
/* Global counter for system-wide use */
uint16_t system_counter = 0;

/* Function with a uniquely named local variable */
void sensor_task(void) {
    uint16_t sensor_task_counter = 0; /* Unique name to avoid shadowing */
    sensor_task_counter++;
}

/* AVOID: Shadowing the global variable */
void incorrect_example(void) {
    uint16_t system_counter = 0; /* Shadows global system_counter */
    system_counter++; /* Modifies local variable, not global */
}
```

## Integer Types

To ensure clarity, avoid using the `int` type as its size varies across platforms. Instead use fixed-size integer types from `<stdint.h>` (eg. `uint16_t`) to explicitly define the size of integer variables.

Always select a type large enough to prevent integer overflows, which can cause undefined behavior in safety-critical applications.

```c
/* Explicitly sized variables */
uint32_t sensor_count = 0; /* 32-bit unsigned for general use */
uint8_t error_code = 0;    /* 8-bit unsigned for limited range (0-255) */

/* AVOID: Ambiguous size */
int count = 0; /* Size depends on platform */
```

## Comments

Every source file must begin with a comment block with our copyright, a high level description of the file's purpose, functionality and its role. If the file controls a specific component or peripheral, include the part number and a link to the component's datasheet for reference. It's recommended to upload the datasheet to Notion to avoid dead links.

```c
/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * EPS Distribution Library
 *
 * Firmware for controlling power distribution on the EPS Distribution board.
 * Functionality includes ...
 */
 
EPSDistrStatus eps_distribution_init(void);
```

Functions should contain a comment directly above the function name with a short description of what the function does. It should also separately describe each parameter and the return value if applicable, especially when the parameter or return value is not obvious. Any constraints or acceptable values for the parameters should be specified in the comment.

```c
/*
 * Takes in the image dimension N, and uses the image API to return the
 * prediction of dimension N.
 *
 * Returns FINCH_IMG_PREDICT_OK on success and FINCH_IMG_PREDICT_FAIL on error.
 */
PredictImageStatus predict_image(
	const vec3 * N,
	int32_t prediction[N->z][N->y][N->x]
);
```

## File and Folder Organization

Our code is structred into the main app and libraries. The main app contains the `main` function for startup and all other code is put into either the `lib` or `drivers` directory. Suppose you are writing the ADCS driver. The structure of your code would be as follows,

```
drivers/
  adcs/
    *.c
include/
  adcs/
    *.h
tests/
  adcs/
    *.c
```

The contents of source files should be organized in the following order,

- `#include` of the matching header file
- `#include` of other header files in our repository, in alphabetical order
- `#include` of standard C libraries (eg. `stdlib.h`), in alphabetical order
- Global variable definitions
- Function definitions

The contents of header files should be organized in the following order:

- `#include` of other header files in our repository, in alphabetical order
- `#include` of standard C libraries, in alphabetical order
- Macro definitions
- Constant definitions
- Enum declarations
- Struct declarations
- Global variable declarations with `extern` (if they need to be publicly visible to other files)
- Function declarations

## Errors

Most functions should return an enum defined in it's header file. The enum may include OK or various ERROR codes. Functions may also log a message on error.

```c
HousekeepingStatus housekeeping_monitor_telem(Subsystem sys)
{
    HousekeepingStatus ret;

    ret = read_subsys(sys);
    if (ret < 0) {
        LOG_ERR("Failed to read subsystem %d with error %d.", (int)sys, ret);
        return HOUSEKEEPING_READ_ERR;
    }

    return HOUSEKEEPING_READ_OK;
}
```
