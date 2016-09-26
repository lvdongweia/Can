/*
 */

#define LOG_TAG "HAL_RM_FLEXCAN"

#include <cutils/log.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* struct ifreq */
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <hardware/rm_flexcan.h>

#define MODULE_NAME            "robot flexcan"
#define MODULE_AUTHOR          "robot"

#define CAN_RTR                 0
#define CAN_FILTER_NUM          1

#define CAN_D_SMOD_OFFSET       0
#define CAN_S_SMOD_OFFSET       4
#define CAN_SUM_OFFSET          8
#define CAN_D_SYS_OFFSET        14
#define CAN_S_SYS_OFFSET        18
#define CAN_PRIORITY_OFFSET     26

/* send */
#define CAN_DATA2ID_PR_MASK     0x7   /* priority   */
#define CAN_DATA2ID_SYS_MASK    0xF0  /* system id  */
#define CAN_DATA2ID_SMOD_MASK   0x0F  /* module id  */
#define CAN_DATA2ID_SUM_MASK    0xFF  /* packet sum */

#define CAN_DATA2ID_PRIORITY(p) ((uint32_t)((p) & CAN_DATA2ID_PR_MASK) << CAN_PRIORITY_OFFSET)
#define CAN_DATA2ID_S_SYS(s)    ((uint32_t)((s) & CAN_DATA2ID_SYS_MASK) << CAN_S_SYS_OFFSET)
#define CAN_DATA2ID_D_SYS(d)    ((uint32_t)((d) & CAN_DATA2ID_SYS_MASK) << CAN_D_SYS_OFFSET)
#define CAN_DATA2ID_S_SMOD(s)   ((uint32_t)((s) & CAN_DATA2ID_SMOD_MASK) << CAN_S_SMOD_OFFSET)
#define CAN_DATA2ID_D_SMOD(d)   ((uint32_t)((d) & CAN_DATA2ID_SMOD_MASK))
#define CAN_DATA2ID_SUM(sum)    ((uint32_t)((sum) & CAN_DATA2ID_SUM_MASK) << CAN_SUM_OFFSET)

/* recv */
#define CAN_ID2DATA_PR_MASK     0x1C000000
#define CAN_ID2DATA_S_SYS_MASK  0x03C00000
#define CAN_ID2DATA_D_SYS_MASK  0x003C0000
#define CAN_ID2DATA_S_SMOD_MASK 0x000000F0
#define CAN_ID2DATA_D_SMOD_MASK 0x0000000F
#define CAN_ID2DATA_SUM_MASK    0x0000FF00

#define CAN_ID2DATA_PRIORITY(p) (uint8_t)(((p) & CAN_ID2DATA_PR_MASK) >> CAN_PRIORITY_OFFSET)
#define CAN_ID2DATA_S_SYS(s)    (uint8_t)(((s) & CAN_ID2DATA_S_SYS_MASK) >> CAN_S_SYS_OFFSET)
#define CAN_ID2DATA_D_SYS(d)    (uint8_t)(((d) & CAN_ID2DATA_D_SYS_MASK) >> CAN_D_SYS_OFFSET)
#define CAN_ID2DATA_S_SMOD(s)   (uint8_t)(((s) & CAN_ID2DATA_S_SMOD_MASK) >> CAN_S_SMOD_OFFSET)
#define CAN_ID2DATA_D_SMOD(d)   (uint8_t)(((d) & CAN_ID2DATA_D_SMOD_MASK))
#define CAN_ID2DATA_SUM(m)      (uint8_t)(((m) & CAN_ID2DATA_SUM_MASK) >> CAN_SUM_OFFSET)
#define CAN_ID2DATA_S_MODULE(s) (CAN_ID2DATA_S_SYS(s) | CAN_ID2DATA_S_SMOD(s))
#define CAN_ID2DATA_D_MODULE(d) (CAN_ID2DATA_D_SYS(d) | CAN_ID2DATA_D_SMOD(d))

/* open and close function */
/* open a specific device, name is device name */
static int flexcan_device_open(const struct hw_module_t* module,
        const char*name,struct hw_device_t** device);
static int _flexcan_device_close(struct hw_device_t* device);

/* device access interface */
static int _flexcan_send(
        struct flexcan_device_t *flexcan_device,
        const struct flexcan_packet *packet);

static int _flexcan_recv(
        struct flexcan_device_t* flexcan_device,
        struct flexcan_packet *packet);

/* module method table */
static struct hw_module_methods_t flexcan_module_methods = {
    open: flexcan_device_open
};

/* module var */
struct flexcan_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: FLEXCAN_HARDWARE_MODULE_ID,
        name: MODULE_NAME,
        author: MODULE_AUTHOR,
        methods: &flexcan_module_methods,
    },
};

static int flexcan_device_open(
        const struct hw_module_t* module,
        const char*name,
        struct hw_device_t** device)
{
    if (strcmp(name, FLEXCAN_HARDWARE_DEVICE_ID)) return -EINVAL;

    struct flexcan_device_t* dev;
    dev = (struct flexcan_device_t*)malloc(sizeof(struct flexcan_device_t));
    if(!dev)
    {
        ALOGE("can device allocate memory!");
        return -EFAULT;
    }

    memset(dev,0,sizeof(struct flexcan_device_t));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)(module);
    dev->common.close = _flexcan_device_close;
    dev->flexcan_send = _flexcan_send;
    dev->flexcan_recv = _flexcan_recv;
    dev->sockfd       = -1;               /* init value */

    /* do something init!! */
    struct ifreq ifr;
    struct sockaddr_can addr;

    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(sock < 0)
    {
        ALOGE("can socket open error: %s", strerror(errno));
        free(dev);
        return -1;
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, ROBOT_CAN_INTERFACE);
    if (ioctl(sock, SIOCGIFINDEX, &ifr))
    {
        ALOGE("can ioctl SIOCGIFINDEX error: %s", strerror(errno));
        close(sock);
        free(dev);
        return -1;
    }

    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        ALOGE("can bind error: %s", strerror(errno));
        close(sock);
        free(dev);
        return -1;
    }

    /* init socket success */
    dev->sockfd = sock;

    ALOGI("can device bind success!!!");
    *device = &(dev->common);
    return 0;
}

static int _flexcan_device_close(struct hw_device_t* device)
{
    struct flexcan_device_t *flexcan_device = (struct flexcan_device_t*)device;
    if (flexcan_device)
    {
        if (flexcan_device->sockfd != -1)
            close(flexcan_device->sockfd);
        free(flexcan_device);
    }

    ALOGI("flexcan device close!!");
    return 0;
}

static int __flexcan_send(
        struct flexcan_device_t* flexcan_device,
        struct can_frame *frame)
{
    int sock = flexcan_device->sockfd;

    /* use extended frame */
    frame->can_id &= CAN_EFF_MASK;
    frame->can_id |= CAN_EFF_FLAG;

    /* unused remote frame: CAN_RTR_FLAG */

    int ret = write(sock, frame, sizeof(struct can_frame));
    if (ret == -1)
    {
        ALOGE("flexcan write error: %s", strerror(errno));
        return -1;
    }

    return 0;
}

static int _flexcan_send(
        struct flexcan_device_t *flexcan_device,
        const struct flexcan_packet *packet)
{
    if (!packet || packet->len <= 0 || packet->len > CAN_DATA_MAX_LEN)
    {
        ALOGE("packet invalid, addr %p, len %d", packet, (!packet ? 0 : packet->len));
        return -1;
    }

    int ret = -1;
    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));

    /* constructure can identify */
    frame.can_id = CAN_DATA2ID_PRIORITY(packet->priority)
        | CAN_DATA2ID_S_SYS(packet->s_id)
        | CAN_DATA2ID_D_SYS(packet->d_id)
        | CAN_DATA2ID_S_SMOD(packet->s_id)
        | CAN_DATA2ID_D_SMOD(packet->d_id);

    /* complete packet */
    if (packet->sum == 0)
    {
        frame.can_dlc = packet->len;
        memcpy(frame.data, packet->data, packet->len);
        ret = __flexcan_send(flexcan_device, &frame);
    }
    else
    {
        /* incomplete packet */
        if (packet->len > CAN_SUB_PACKET_SIZE)
        {
            ALOGE("packet is incomplete, but data length > %d", CAN_DATA_MAX_LEN - 2);
            return -1;
        }

        /* starting from 0(sum + 1)*/
        frame.can_id |= CAN_DATA2ID_SUM(packet->sum);

        memset(frame.data, 0, sizeof(frame.data));
        frame.data[0] = packet->packet_id;
        frame.data[1] = packet->packet_SN;
        memcpy(frame.data + CAN_SUB_PACKET_OFFSET, packet->data, packet->len);
        frame.can_dlc = packet->len + CAN_SUB_PACKET_OFFSET;

        if ((ret = __flexcan_send(flexcan_device, &frame)) == -1)
            return -1;
    }

    return ret;
}

static int __flexcan_recv(
        struct flexcan_device_t *flexcan_device,
        struct can_frame *frame)
{
    int sock = flexcan_device->sockfd;

    /* A filter matches, when
     * <received_can_id> & mask == can_id & mask
     */
    /*
    struct can_filter filter[CAN_FILTER_NUM];
    memset(&filter, 0, sizeof(struct can_filter) * CAN_FILTER_NUM);
    filter[0].can_id = xxx;
    filter[0].can_mask = CAN_EFF_MASK;

    if (setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
                filter, sizeof(rfilter) * CAN_FILTER_NUM) != 0)
    {
        ALOGE("[flexcan]--setsockopt error");
        return -1;
    }
    */

    int32_t nbytes = read(sock, frame, sizeof(struct can_frame));
    if (nbytes < 0)
    {
        ALOGE("flexcan read error: %s", strerror(errno));
        return -1;
    }
    else if (nbytes != sizeof(struct can_frame))
    {
        ALOGE("read: incomplete CAN frame");
        return -1;
    }
    else if (frame->can_id & CAN_ERR_FLAG)
    {
        ALOGE("read: error frame");
        return -1;
    }

    return 0;
}

static int isBroadCast(void)
{
    return 0;
}

static int _flexcan_recv(
        struct flexcan_device_t* flexcan_device,
        struct flexcan_packet *packet)
{
    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));

    int ret = __flexcan_recv(flexcan_device, &frame);
    if (ret != 0) return -1;

    /* get frame real id */
    if (!(frame.can_id & CAN_EFF_FLAG))
    {
        ALOGE("frame is not extended frame, can_id 0x%08X", frame.can_id);
        return -1;
    }
    uint32_t extid = frame.can_id & CAN_EFF_MASK;

    /* parse frame id */
    packet->priority = CAN_ID2DATA_PRIORITY(extid); /* priority       */
    packet->s_id     = CAN_ID2DATA_S_MODULE(extid); /* src module id  */
    if (isBroadCast())                              /* dest module id */
        packet->d_id = 0xFF;
    else
        packet->d_id = CAN_ID2DATA_D_MODULE(extid);

    uint32_t sum = CAN_ID2DATA_SUM(extid);
    packet->sum = sum;

    /* set 0 in external */
    //memset(packet->data, 0, CAN_DATA_MAX_LEN);

    /* just one can packet */
    if (0 == sum)
    {
        memcpy(packet->data, frame.data, frame.can_dlc);

        packet->len = frame.can_dlc;
        packet->packet_id = 0;
        packet->packet_SN = 0;
        return 0;
    }

    /* more packets, incomplete packet */
    if (frame.can_dlc < (CAN_SUB_PACKET_OFFSET + 1))
    {
        ALOGE("can frame sub-packet length error: %d", frame.can_dlc);
        return -1;
    }

    memcpy(packet->data, frame.data + CAN_SUB_PACKET_OFFSET,
            frame.can_dlc - CAN_SUB_PACKET_OFFSET);

    packet->len = frame.can_dlc - CAN_SUB_PACKET_OFFSET;
    packet->packet_id = frame.data[0];
    packet->packet_SN = frame.data[1];

    return 0;
}
