/**
 *  @file 
 * @brief Idle command sequence implementation 
*/

#include "idle.h"
#include <stdbool.h>

/** 
 * @brief OBC transmits spacecraft orient command
*/
int obc_command_orient(const char* attitude) {
    // sim transmission
    return 0;
}

/** 
 * @brief Orient satellite through ADCS
*/
static bool adcs_execute_orient(const char* attitude) {
    // sim execution 
    return true; 
}

/** 
 * @brief Performs internal system health check on OBC 
*/
static bool obc_system_health_check(void) {
    //add internal logic
    return true;
}

/**
 * @brief OBC checks for scheduled tasks
*/
static bool obc_check_scheduled(void) {
    // sim check
    return true;
}

/**
 * @brief OBC Schedules mode change
*/ 
int obc_scheduled_mode_change(const char* command_mode, const char* mode_params, const char* schedule) {
    // sim succcessful scheduling
    return 0;
}

/**
 * @brief OBC logs error
*/
int obc_log_error(const char* errortype) {
    // Sim logging
    return 0;
}


/**
 * @brief if contact is not established between RF and OBC
*/
int rf_msg_error(const char* errortype) {
    // sim transmission 
    return 0;
}

/** 
 * @brief PAY enters specified mode 
*/
int pay_enter_mode(const char* command_mode) {
    // sim mode entry 
    return 0;
}

/**
 * @brief RF transmits command to OBC
*/
static bool rf_transmit_command_modechange(const char* command_mode, const char* mode_params, const char* schedule) {
    // sim transmission
    return true; 
}

/**
 * @brief MCC transmits command to RF
*/
int mcc_transmit_command_modechange(const char* command_mode, const char* mode_params, const char* schedule) {
    // sim transmission
    return 0;
}

/**
 * @brief Main loop handling the idle command sequence logic
*/
int idle_command_sequence(void) {
    const char* attitude = "sunpointing";
    const char* command_mode = "cmdmode";
    const char* mode_params = "modeparams";
    const char* schedule = "scheduled";
    const char* errortype = "comms";

    obc_command_orient("sunpointing");
    if (adcs_execute_orient("sunpointing") == true) {
        if (obc_system_health_check() == false) {
            pay_enter_mode("safety");
        } 
    } else {
        pay_enter_mode("safety");
    }

    if (obc_check_scheduled() == true) {
        // sim scheduled 
        pay_enter_mode("cmdmode");
    } 

    mcc_transmit_command_modechange("cmdmode", "modeparams", "scheduled");

    if (rf_transmit_command_modechange("cmdmode", "modeparams", "scheduled") == true) {
        if (obc_scheduled_mode_change("cmdmode", "modeparams", "scheduled") == true) {
            pay_enter_mode("cmdmode");
        } else {
            obc_log_error("comms");
        }
    } else {
        rf_msg_error("comms");
        obc_log_error("comms");
        pay_enter_mode("safety");
    }
    return 0;
}
