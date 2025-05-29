/** @file idle.h
 * @brief Header for Idle Command Sequene Logic
*/

#ifndef idle_h
#define idle_h

#include <stdbool.h>

/**
 * @brief system modules involved in idle command sequence 
*/
typedef enum system_module{
    MODULE_OPERATOR,
    MODULE_MCC_GS,
    MODULE_RF, 
    MODULE_OBC,
    MODULE_ADCS, 
    MODULE_PAY
} system_module; 

/** 
 * @brief executes the idle command sequence 
*/
int idle_command_sequence();

#endif
