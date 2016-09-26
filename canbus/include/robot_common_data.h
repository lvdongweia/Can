/*
 * File Name: robot_module_id.h
 * Autor: jinwen.gao@archermind.com
 * Data: 2014-10-9
 * Description:
 * All modules ID/Can Priority definition in robot system
 *
 * Note: *****
 * if updata this file, must sync the follow files:
 * libcan/robot_module_name.cpp
 * rm_fault_manager/librm_fault/robot_fault_defs.h
 */

#ifndef ROBOT_MODULE_ID_H_
#define ROBOT_MODULE_ID_H_

/* Note: system id is the high four bit of module id */
enum RobotSystemId
{
    RM_SYSTEM_ID             = 0x0,
    RC_SYSTEM_ID             = 0x1,
    RP_SYSTEM_ID             = 0x2,
    RF_SYSTEM_ID             = 0x3,
    RB_R_ARM_ID              = 0x4,
    RB_L_ARM_ID              = 0x5,
    RB_BODY_ID               = 0x6,
    R_RC_ID                  = 0x7,
};

/************************************ module id **********************************/
/* robot PAD system module id */
enum RmModuleId
{
    RM_SYSCTRL_ID            = 0x00,
    RM_MOTOR_ID              = 0x01,
    RM_FAULT_ID              = 0x02,
    RM_DEBUG_ID              = 0x03,
    RM_FACE_ID               = 0x04,
    RM_SENSOR_ID             = 0x05,
    RM_SCHEDULER_ID          = 0x06,
    RM_POWER_ID              = 0x07,
    RM_DEBUG_CTRL_ID         = 0x08,
    RM_FACTORY_TEST_ID       = 0x09,
    RM_UPGRADE_ID            = 0x0A,
    RM_MODULE_NUM
};

/* robot Motion Control system module id */
enum RcModuleId
{
    RC_SYS_CTRL_ID           = 0x10,
    RC_BASE_CTRL_ID          = 0x11,
    RC_TRAJ_CTRL_ID          = 0x12,
    RC_TASK_CTRL_ID          = 0x13,
    RC_SENSOR_ID             = 0x14,
    RC_FAULT_ID              = 0x15,
    RC_DEBUG_ID              = 0x16,
    RC_EVENT_DETECT_ID       = 0x17,
    RC_UPGRADE_ID            = 0x18,
    RC_MODULE_NUM            = 0x8
};

/* robot power manager system module id */
enum RpModuleId
{
    RP_SYSCTRL_ID            = 0x20,
    RP_CHARGE_ID             = 0x21,
    RP_CONTROL_ID            = 0x22,
    RP_FAULT_ID              = 0x23,
    RP_DEBUG_ID              = 0x24,
    RP_UPGRADE_ID            = 0x25,
    RP_MODULE_NUM            = 0x5,
};

/* robot face control system module id */
enum RfModuleId
{
    RF_SYSCTRL_ID            = 0x30,
    RF_SENSOR_ID             = 0x31,
    RF_DISPLAY_ID            = 0x32,
    RF_FAULT_ID              = 0x33,
    RF_DEBUG_ID              = 0x34,
    RF_UPGRADE_ID            = 0x35,
    RF_MODULE_NUM            = 0x5,
};

/* robot right arm servo system module id */
enum RbRArmModuleId
{
    RB_R_ARM_SYS_CTRL_ID     = 0x40,
    RB_R_ARM_MOTOR_CTRL_ID   = 0x41,
    RB_R_ARM_SENSOR_ID       = 0x42,
    RB_R_ARM_DEBUG_ID        = 0x43,
    RB_R_ARM_FAULT_ID        = 0x44,
    RB_R_ARM_EVENT_DETECT_ID = 0x45,
    RB_R_ARM_UPGRADE_ID      = 0x46,
    RB_R_ARM_MODULE_NUM      = 0x06
};

/* robot left arm servo system module id */
enum RbLArmModuleId
{
    RB_L_ARM_SYS_CTRL_ID     = 0x50,
    RB_L_ARM_MOTOR_CTRL_ID   = 0x51,
    RB_L_ARM_SENSOR_ID       = 0x52,
    RB_L_ARM_DEBUG_ID        = 0x53,
    RB_L_ARM_FAULT_ID        = 0x54,
    RB_L_ARM_EVENT_DETECT_ID = 0x55,
    RB_L_ARM_UPGRADE_ID      = 0X56,
    RB_L_ARM_MODULE_NUM      = 0x06
};

/* robot body servo system module id */
enum RbBodyModuleId
{
    RB_BODY_SYS_CTRL_ID      = 0x60,
    RB_BODY_MOTOR_CTRL_ID    = 0x61,
    RB_BODY_SENSOR_ID        = 0x62,
    RB_BODY_DEBUG_ID         = 0x63,
    RB_BODY_FAULT_ID         = 0x64,
    RB_BODY_EVENT_DETECT_ID  = 0x65,
    RB_BODY_UPGRADE_ID       = 0x66,
    RB_BODY_MODULE_NUM       = 0x06
};

/* broadcast id */
enum RobotCastId
{
    ROBOT_RADIOCAST_ID       = 0x70,
};

/********************************* can priority *******************************
 * ROBOT_CAN_LEVEL0 is highest level
 * ROBOT_CAN_LEVEL6 is lowest level
 */
enum RobotCanPriority
{
    ROBOT_CAN_LEVEL0            = 0,
    ROBOT_CAN_LEVEL1            = 1,
    ROBOT_CAN_LEVEL2            = 2,
    ROBOT_CAN_LEVEL3            = 3,
    ROBOT_CAN_LEVEL4            = 4,
    ROBOT_CAN_LEVEL5            = 5,
    ROBOT_CAN_LEVEL6            = 6,
};

#endif /* ROBOT_MODULE_ID_H_ */
