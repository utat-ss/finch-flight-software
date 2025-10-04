/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "obc_check.h"
#include <stdbool.h>
#include <stdio.h>

/* 
 * static functions keep internal logic here
*/

// individual node pings (Level 3)
static bool ping_srs4(void) {
    printf("Pinging SRS4...\n");
    // TODO: implement actual logic
    return true; // simulated success
}

static bool ping_eps(void) {
    printf("Pining EPS (Power)...\n");
    // TODO: actual logic
    return true; // simulated success
}

static bool ping_10m(void) {
    printf("Pinging ADCS...\n");
    // TODO: actual logic
    return true; // simulated success 
}

// Level 2
static bool ping_all_CSP_nodes(void) {
    bool srs4_ok = ping_srs4(); 
    bool EPS_ok = ping_eps(); 
    bool ADCS_ok = ping_10m(); 

    return srs4_ok && EPS_ok && ADCS_ok; 
}

static bool flash_check(void) {
    printf("checking flash memory...\n");
    // TODO: logic
    return true; // simnulated success
}

static bool external_RTC_check(void) {
    printf("Checking external RTC...\n");
    // TODO: logic
    return true; // sim success 
}

// level 1
bool obc_check(void) {
    bool ping_ok = ping_all_CSP_nodes;
    bool flash_ok = flash_check;
    bool RTC_ok = external_RTC_check;

    return ping_ok && flash_ok && RTC_ok;
}
