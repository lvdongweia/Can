/*
 */

#ifndef RM_CAN_DATA_TYPE_H_
#define RM_CAN_DATA_TYPE_H_

#include "robot_common_data.h"

/********************************** robot log level *****************************/

enum RobotLogLevel
{
    ROBOT_LOG_DEBUG     = 0,
    ROBOT_LOG_INFO      = 1,
    ROBOT_LOG_WARN      = 2,
    ROBOT_LOG_ERROR     = 3,
    ROBOT_LOG_FATAL     = 4,
    ROBOT_LOG_LEVEL_MAX,
};

/*use: (ROBOT_DEBUG_LEVEL | ROBOT_INFO_LEVEL) */
enum RobotLogOnOffFlag
{
    ROBOT_DEBUG_LEVEL = 1 << 0,   /* 0x01 */
    ROBOT_INFO_LEVEL  = 1 << 1,   /* 0x02 */
    ROBOT_WARN_LEVEL  = 1 << 2,   /* 0x04 */
    ROBOT_ERROR_LEVEL = 1 << 3,   /* 0x08 */
    ROBOT_FATAL_LEVEL = 1 << 4,   /* 0x10 */
};

/********************************** robot fault level **************************/

enum RobotFaultLevel
{
    ROBOT_FAULT_WARN    = 0,
    ROBOT_FAULT_ERROR   = 1,
    ROBOT_FAULT_FATAL   = 2,
};

#endif /* RM_CAN_DATA_TYPE_H_ */
