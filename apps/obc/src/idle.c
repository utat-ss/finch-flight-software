/** 
 * @startuml //need plantuml support to be able to generate diagram from mermaid code comments 
 *  
 * 
*/



/** @file idle_sequence.c
 * @brief Implements the Idle command sequence 
*/

#include <stdio.h>
#include <stdbool.h>

/// System modules not acc used within but just tho keep track j'ai les adjoute
typedef enum SystemModule{
    MODULE_OPERATOR,
    MODULE_MCC_GS,
    MODULE_RF,
    MODULE_OBC,
    MODULE_ADCS, 
    MODULE_PAY
} SystemModule;


/// rep command status
typedef enum CommandStatus{
    STATUS_SUCCESS,
    STATUS_ERROR_COM
} CommandStatus; 

/// Orientation command ADCS
/** 
 * @brief Orient satellite thru ADCS
 * @param attitude Target Attitude (eg Sun pointing)
 * @return STATUS_SUCCESS if completed
*/
CommandStatus ADCS_ExecuteOrient(const char* attitude) {
    // sim execution 
    return STATUS_SUCCESS; // this oe has a feedback loop which is why it has the commandstatus types
}

/// Checks system health on OBC 
/** 
 * @brief Performs internal system health check 
*/
void OBC_SystemHealthCheck(void){
    //internal logic idk wtf goes on here sorry dawgs
}

/// OBC checks for scheduled tasks
bool OBC_CheckScheduled(void) {
    // sim check
    return true;
}

/// OBC Schedules mode change
void OBC_ScheduledModeChange(const char* mode, const char* params) {
    // sim scheduling // im a little confused about the parameters, might need to ask ceci should it be (const char* param, const char* mode, const char* schedule)
}

/// OBC logs an error
void OBC_LogError(const char* errorType) {
    // Sim logging
}

/// PAY enters specified mode 
void PAY_EnterMode(const char* mode) {
    // sim mode entry 
}

/// RF transmits command to OBC
bool RF_TransmitCommand(const char* command, const char* params) {
    // sim transmission
    return true; // sim successful 
}

/// Operator sends command to MCC/GS
void Operator_SendCommand(const char* command, const char* params) {
    // sim sending command
}

/// MCC sends command to RF
void MCC_TransmitToRF(const char* command, const char* params) {
    // sim transmission
}

/**
 * @brief Main loop handling the idle command sequence logic
*/
void IdleCommandSequence() {
    const char* attitude = "SunPointing";
    const char* mode = "cmdMode";
    const char* schedule = "scheduled";

    // Orientation Sequence: OBC -> ADCS -> OBC
    if (ADCS_ExecuteOrient(attitude) == STATUS_SUCCESS) {
        OBC_SystemHealthCheck();
    }

    // Loop: Wait for ping (simplified as single iteration)
    if (OBC_CheckScheduled()) {
        // sim scheduled 
        PAY_EnterMode("schMode");
    }

    // Operator initiates a mode change command
    Operator_SendCommand(mode, schedule);
    MCC_TransmitToRF(mode, schedule);

    // RF transmits to OBC
    if (RF_TransmitCommand(mode, schedule)) {
        bool isScheduled = true; // or false to trigger immediate execution
        if (isScheduled) {
            OBC_ScheduledModeChange(mode, schedule);
        } else {
            PAY_EnterMode(mode);
        }
    } else {
        // Error condition 
        OBC_LogError("Communication");
        PAY_EnterMode("Safety");
    }
}

