/*
 */

#include <stdio.h>
#include <unistd.h>

#include "rm_can_data_type.h"
#include "RobotModuleName.h"

#define STR(module) #module

static struct module_name_map
{
    int8_t module_id;
    const char *module_name;
}RmModuleNameMap[] =
{
    {RM_SYSCTRL_ID,     STR(RM_SYSCTRL_ID)},
    {RM_MOTOR_ID,       STR(RM_MOTOR_ID)},
    {RM_FAULT_ID,       STR(RM_FAULT_ID)},
    {RM_DEBUG_ID,       STR(RM_DEBUG_ID)},
    {RM_FACE_ID,        STR(RM_FACE_ID)},
    {RM_SENSOR_ID,      STR(RM_SENSOR_ID)},
    {RM_SCHEDULER_ID,   STR(RM_SCHEDULER_ID)},
    {RM_POWER_ID,       STR(RM_POWER_ID)},
    {RM_DEBUG_CTRL_ID,  STR(RM_DEBUG_CTRL_ID)},
    {RM_FACTORY_TEST_ID,STR(RM_FACTORY_TEST_ID)},
    {RM_UPGRADE_ID,     STR(RM_UPGRADE_ID)},
}, RcModuleNameMap[] =
{
    {RC_SYS_CTRL_ID,    STR(RC_SYS_CTRL_ID)},
    {RC_BASE_CTRL_ID,   STR(RC_BASE_CTRL_ID)},
    {RC_TRAJ_CTRL_ID,   STR(RC_TRAJ_CTRL_ID)},
    {RC_TASK_CTRL_ID,   STR(RC_TASK_CTRL_ID)},
    {RC_SENSOR_ID,      STR(RC_SENSOR_ID)},
    {RC_FAULT_ID,       STR(RC_FAULT_ID)},
    {RC_DEBUG_ID,       STR(RC_DEBUG_ID)},
    {RC_EVENT_DETECT_ID,STR(RC_EVENT_DETECT_ID)},
    {RC_UPGRADE_ID,     STR(RC_UPGRADE_ID)},
}, RpModuleNameMap[] =
{
    {RP_SYSCTRL_ID,     STR(RP_SYSCTRL_ID)},
    {RP_CHARGE_ID,      STR(RP_CHARGE_ID)},
    {RP_CONTROL_ID,     STR(RP_CONTROL_ID)},
    {RP_FAULT_ID,       STR(RP_FAULT_ID)},
    {RP_DEBUG_ID,       STR(RP_DEBUG_ID)},
    {RP_UPGRADE_ID,     STR(RP_UPGRADE_ID)},
}, RfModuleNameMap[] =
{
    {RF_SYSCTRL_ID,     STR(RF_SYSCTRL_ID)},
    {RF_SENSOR_ID,      STR(RF_SENSOR_ID)},
    {RF_DISPLAY_ID,     STR(RF_DISPLAY_ID)},
    {RF_FAULT_ID,       STR(RF_FAULT_ID)},
    {RF_DEBUG_ID,       STR(RF_DEBUG_ID)},
    {RF_UPGRADE_ID,     STR(RF_UPGRADE_ID)},
}, RbRArmModuleNameMap[] =
{
    {RB_R_ARM_SYS_CTRL_ID,      STR(RB_R_ARM_SYS_CTRL_ID)},
    {RB_R_ARM_MOTOR_CTRL_ID,    STR(RB_R_ARM_MOTOR_CTRL_ID)},
    {RB_R_ARM_SENSOR_ID,        STR(RB_R_ARM_SENSOR_ID)},
    {RB_R_ARM_DEBUG_ID,         STR(RB_R_ARM_DEBUG_ID)},
    {RB_R_ARM_FAULT_ID,         STR(RB_R_ARM_FAULT_ID)},
    {RB_R_ARM_EVENT_DETECT_ID,  STR(RB_R_ARM_EVENT_DETECT_ID)},
    {RB_R_ARM_UPGRADE_ID,       STR(RB_R_ARM_UPGRADE_ID)},
}, RbLArmModuleNameMap[] =
{
    {RB_L_ARM_SYS_CTRL_ID,      STR(RB_L_ARM_SYS_CTRL_ID)},
    {RB_L_ARM_MOTOR_CTRL_ID,    STR(RB_L_ARM_MOTOR_CTRL_ID)},
    {RB_L_ARM_SENSOR_ID,        STR(RB_L_ARM_SENSOR_ID)},
    {RB_L_ARM_DEBUG_ID,         STR(RB_L_ARM_DEBUG_ID)},
    {RB_L_ARM_FAULT_ID,         STR(RB_L_ARM_FAULT_ID)},
    {RB_L_ARM_EVENT_DETECT_ID,  STR(RB_L_ARM_EVENT_DETECT_ID)},
    {RB_L_ARM_UPGRADE_ID,       STR(RB_L_ARM_UPGRADE_ID)},
}, RbBodyModuleNameMap[] =
{
    {RB_BODY_SYS_CTRL_ID,      STR(RB_BODY_SYS_CTRL_ID)},
    {RB_BODY_MOTOR_CTRL_ID,    STR(RB_BODY_MOTOR_CTRL_ID)},
    {RB_BODY_SENSOR_ID,        STR(RB_BODY_SENSOR_ID)},
    {RB_BODY_DEBUG_ID,         STR(RB_BODY_DEBUG_ID)},
    {RB_BODY_FAULT_ID,         STR(RB_BODY_FAULT_ID)},
    {RB_BODY_EVENT_DETECT_ID,  STR(RB_BODY_EVENT_DETECT_ID)},
    {RB_BODY_UPGRADE_ID,       STR(RB_BODY_UPGRADE_ID)},
};

/* rm */
static const char *RmGetRmModuleName(int index)
{
    int len = sizeof(RmModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RmModuleNameMap[index].module_name;
    else
        return "Unknown RmModule Name";
}

/* rc */
static const char *RmGetRcModuleName(int index)
{
    int len = sizeof(RcModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RcModuleNameMap[index].module_name;
    else
        return "Unknown RcModule Name";
}

/* rp */
static const char *RmGetRpModuleName(int index)
{
    int len = sizeof(RpModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RpModuleNameMap[index].module_name;
    else
        return "Unknown RpModule Name";
}

/* rf */
static const char *RmGetRfModuleName(int index)
{
    int len = sizeof(RfModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RfModuleNameMap[index].module_name;
    else
        return "Unknown RfModule Name";
}

/* rb_r_arm */
static const char *RmGetRbRArmModuleName(int index)
{
    int len = sizeof(RbRArmModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RbRArmModuleNameMap[index].module_name;
    else
        return "Unknown RbRArmModule Name";
}

/* rb_l_arm */
static const char *RmGetRbLArmModuleName(int index)
{
    int len = sizeof(RbLArmModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RbLArmModuleNameMap[index].module_name;
    else
        return "Unknown RbLArmModule Name";
}

/* rb body */
static const char *RmGetRbBodyModuleName(int index)
{
    int len = sizeof(RbBodyModuleNameMap) / sizeof(struct module_name_map);
    if (index >= 0 && index < len)
        return RbBodyModuleNameMap[index].module_name;
    else
        return "Unknown RbBodyModule Name";
}

/* robot radiocast */
static const char *RmGetRRcModuleName(int module_id)
{
    switch (module_id)
    {
        case ROBOT_RADIOCAST_ID:
            return STR(ROBOT_RADIOCAST);
        default:
            return "Unknown RRcModule Name";
    }

    return NULL;
}

const char* RmGetModuleName(int module_id)
{
    int system_id = module_id >> 4;
    int index = module_id & 0x0F;

    switch (system_id)
    {
        case RM_SYSTEM_ID:
            return RmGetRmModuleName(index);
        case RC_SYSTEM_ID:
            return RmGetRcModuleName(index);
        case RP_SYSTEM_ID:
            return RmGetRpModuleName(index);
        case RF_SYSTEM_ID:
            return RmGetRfModuleName(index);
        case RB_R_ARM_ID:
            return RmGetRbRArmModuleName(index);
        case RB_L_ARM_ID:
            return RmGetRbLArmModuleName(index);
        case RB_BODY_ID:
            return RmGetRbBodyModuleName(index);
        case R_RC_ID:
            return RmGetRRcModuleName(module_id);
        default:
            return "Unknown Module Name";
    }

    return NULL;
}
