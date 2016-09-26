/*
*/

#ifndef HARDWARE_RM_FLEXCAN_H_
#define HARDWARE_RM_FLEXCAN_H_

#include <unistd.h>
#include <hardware/hardware.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

/* define module id */
#define FLEXCAN_HARDWARE_MODULE_ID "flexcan"

/* There are multiple devices in one module.
 * Only one device in flexcan module.
 */
#define FLEXCAN_HARDWARE_DEVICE_ID "flexcan"

#define ROBOT_FLEXCAN_API_VERSION  20140925

#define ROBOT_CAN_INTERFACE        "can0"

#define PACKET_MAX_NUM             256
#define CAN_DATA_MAX_LEN           8
#define CAN_SUB_PACKET_OFFSET      2       /* 0: id; 1: SN */
#define CAN_SUB_PACKET_SIZE        (CAN_DATA_MAX_LEN - CAN_SUB_PACKET_OFFSET)
#define CAN_DATA_TOTAL_SIZE        (CAN_SUB_PACKET_SIZE * PACKET_MAX_NUM)

/* hardware module struct */
struct flexcan_module_t
{
    struct hw_module_t common;
};

/*
 * Note: if packet will send and is incomplete,
 * @len <= CAN_DATA_MAX_LEN - 2
 * data[0] = packet_id;
 * data[1] = packet_no;
 *
 * .sum == 0:
 * packet is complete.
 * .sum > 0:
 * packet is incomplete.
 */
struct flexcan_packet
{
    uint8_t packet_id;             /* uniquely identifies a entry */
    uint8_t priority;
    uint8_t s_id;
    uint8_t d_id;
    uint8_t data[CAN_DATA_MAX_LEN];
    uint32_t len;                  /* real data len <= CAN_DATA_MAX_LEN */
    uint8_t packet_SN;             /* packets number */
    uint8_t sum;                   /* starting from 0 */
};

/* hardware interface struct */
struct flexcan_device_t
{
    struct hw_device_t common;

    int sockfd;

    int (*flexcan_send)(
            struct flexcan_device_t* flexcan_device,
            const struct flexcan_packet *packet);

    int (*flexcan_recv)(
            struct flexcan_device_t* flexcan_device,
            struct flexcan_packet *packet);
};

__END_DECLS

#endif /* HARDWARE_RM_FLEXCAN_H_ */
