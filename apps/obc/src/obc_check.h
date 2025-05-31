#ifndef OBC_CHECK_H
#define OBC_CHECK_H

#include <stdbool.h>

/** 
 * @brief Runs the full OBC system check 
 * 
 * Checks: 
 * - Ping all CSP nodes (RF, ADCS, EPS)
 * - Flash memory health 
 * - External RTC functionality 
 * 
 * @return true is all checks pass, fail otherwise
*/

bool obc_check(void);

#endif 
