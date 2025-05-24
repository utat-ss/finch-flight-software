/** @file idle.h
 * @brief Header for Idle Command Sequene Logic
*/

#ifndef IDLE_H
#define IDLE_H

#include <stdbool.h>

/// Status type for command sequences
typedef enum CommandStatus{
    STATUS_SUCCESS,
    STATUS_ERROR_COM
} CommandStatus; 

/// System modules
typedef enum SystemModule{
    MODULE_OPERATOR,
    MODULE_MCC_GS,
    MODULE_RF, 
    MODULE_OBC,
    MODULE_ADCS, 
    MODULE_PAY
} SystemModule;

CommandStatus ADCS_ExecuteOrient(const char* attitude);
void OBC_SystemHealthCheck();
bool OBC_CheckScheduled(void);
void OBC_ScheduledModeChange(const char* mode, const char* params);
void OBC_LogError(const char* errorType);
void PAY_EnterMode(const char* mode);
bool RF_TransmitCommand(const char* command, const char* params);
void Operator_SendCommand(const char* command, const char* params);
void IdleCommandSequence();



#endif

