*A living document of best practices for writing firmware.*

*For coding style guidelines, see the [Zephyr Style Guide](https://docs.zephyrproject.org/latest/contribute/style/index.html).*

## Memory Allocation

The **malloc()** function is not permitted to be used and any dynamic memory allocation should be avoided after initialization. It is incredibly easy to mishandle memory and free routines, and they are unpredictable as to how much time they take to execute. Therefore, they are not safe for safety-critical applications.

All arrays must be statically declared using a fixed number of elements.

## Infinite Loops

Do not allow the possibility of infinite loops. All loops must have a fixed upper bound that can be statically proven and determined. A separate timeout should always be added for code waiting for a hardware event so that the MCU does not stall.

Here is a good illustration showing a loop that waits for the PB0 pin to go low.

```c
/* built-in UINT16_MAX constant starts at 2^16 - 1 */
uint16_t timeout = UINT16_MAX;
while (bit_is_set(PINB, PB0) && timeout > 0) {
    timeout--
}
```

---

## Volatile Variables

For any variables that are modified in an interrupt handler, they must be declared using the **volatile** keyword.

## Scope

Variables should be declared at the lowest scope possible. Variable names used in a larger scope should never be repeated for separate variables in a smaller scope in order to avoid variable eclipsing bugs that can be subtle but nasty and hard to detect.

```c
/* AVOID */
uint16_t counter = 0;
 
void func(void) {
    uint16_t counter = 0;
}
 
/* Correct */
uint16_t counter = 0;
 
void func(void) {
    uint16_t counter_func = 0;
}
```

---

## Integer Types

Never use the int type. All integer variables must be declared with explicit sizes to be clear about the maximum value the integer can hold and support. Instead, use types such as `uint8_t`, `uint16_t`, and `uint32_t` so that the size of the integer value is clear and obvious. If you are unsure as to how large an integer variable should be, use `uint32_t` by default since the MCU architecture is 32-bit.

It should be noted that integer overflows must be avoided at all times, so make sure that the integer types used are large enough to hold the values they contain.

## Compiler Warnings

Correct and functional code should not produce any compiler warnings when compiled. Essentially, follow the saying "treat warnings as errors". At a minimum, any code merged into the main branch on Git should compile with 0 errors and 0 warnings.

It's easy to ignore warnings, but warnings often give you hints about potential or definite bugs in your code. If your code isn't working, try cleaning and recompiling your project, then see if it produces any warnings and fix those warnings if they exist.

## Initializing Structs

When writing a function to initialize a struct (e.g. for the UART struct this would be the uart_init() function), always pass the struct to be initialized as the first parameter to the function. Do not initialize a new struct variable within the function and return the value from the function.

The method of returning a struct value from the function looks like this:

```c
UART uart_init(...) {
    UART new_uart;
    ...
    return new_uart;
}
```

---

This correctly initializes the fields of the struct, however there is a key issue with this approach. This method creates a UART struct on the stack of the uart_init function, then returns a COPY of that struct to the calling code. This means the uart_init function cannot get a pointer to the struct in the calling code that the result is being copied to. When writing firmware, you often need the pointer to the struct in the calling code within the initialization function, e.g. to assign it to a global variable or to a field of another struct. You could get the pointer to the new_uart struct (i.e. the address of new_uart on the stack), but new_uart is destroyed after the function completes and it copies the return value to the calling code, so that pointer is invalid outside of the uart_init function.

Therefore, you should always write your initialization functions with this method:

```c
void uart_init(UART* uart, ...) {
    ...
}
```

---

This way, the uart_init function has access to the pointer of the UART struct in the calling code, where the initialized results will be saved.

Not every struct initialization function needs a pointer to the struct in the calling code, but we should always write struct initialization functions this way for consistency.

## Comments

At the beginning of each file, there should be a comment that contains a high level description of the purpose of the code. If the code in a file is used to control a particular component/peripheral, the part number of that component should be listed alongside a link to its datasheet.

```c
/*
MCP23S17 port expander (PEX)
Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/20001952C.pdf
 
**High level description of program and how it controls the part listed above
*/
 
int main() {
    return 0;
}
```

---

Functions should contain a comment directly above the function name with a short description of what the function does. It should also separately describe each parameter and the return value if applicable, especially when the parameter or return value is not obvious. Any constraints or acceptable values for the parameters should be specified in the comment.

```c
/*
Interrupt service routine that releases three semaphores once the letter 'r' has been sent
to the UART
 
huart - UART handle
Returns - none
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    HAL_UART_Receive_IT(huart, &rx_data, 1);
 
    if (rx_data == 'r') {
        BaseType_t xHigherPriorityTaskWoken = pdFalse;
 
        xSemaphoreGiveFromISR(CountingSem, &xHigherPriorityTaskWoken);
        xSemaphoreGiveFromISR(CountingSem, &xHigherPriorityTaskWoken);
        xSemaphoreGiveFromISR(CountingSem, &xHigherPriorityTaskWoken);
    }
}
```

---

## File and Folder Organization

Suppose you are writing the `adcs` library. The structure of your code would be as follows,

```
lib/
  adcs/
    *.c
include/
  adcs/
    *.h
tests/
  adcs/
    *.c
```

The contents of .c/.cpp files should be organized in the following order:

- `#include` of the matching .h/.hpp file
- `#include` of other .h/.hpp files in our repository, in alphabetical order
- `#include` of standard C libraries (e.g. `stdlib.h`), in alphabetical order
- Global variable definitions
- Function definitions

The contents of .h/.hpp files should be organized in the following order:

- `#include` of other .h/.hpp files in our repository, in alphabetical order
- `#include` of standard C libraries (e.g. `stdlib.h`), in alphabetical order
- Macro definitions
- Constant definitions
- Enum declarations
- Struct/class declarations
- Global variable declarations with `extern` (if they need to be publicly visible to other files)
- Function declarations

A file should first contain a prologue to the file, which may include a large comment containing a description of the code. Functions should follow a breadth-first approach where functions are grouped based on a similar level of abstraction.

# **Testing Guidelines**

## Encapsulate Code That You Expect to Change

Code that is properly encapsulated is easier to test and maintain. Encapsulation can be implemented in several ways including organizing a program into files, organizing files into data sections and function sections, organizing functions into logically related groups, and organizing data into logical groups (data structures).

## Writing Good Unit Tests

When developing unit tests, they should not only cover cases where an operation succeeds, but also check cases where an operation should fail (e.g. invalid message, array/queue is full, passed a NULL pointer). Unit tests should be fast and easily repeatable with consistent results. Furthermore, they should be able to run in isolation with no dependencies on any outside factors. The test should be able to automatically check whether it has passed or failed without human interaction. Finally, try to test multiple operations in different orders, not always in the same sequence.
