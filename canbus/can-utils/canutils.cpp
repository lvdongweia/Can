#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "canutils.h"

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

struct can_packet
{
    uint8_t priority;
    uint8_t s_id;
    uint8_t d_id;
    uint8_t len;
    uint8_t data[8];
};

static int char2Int(char c)
{
    if (c >= '0' && c <= '9')
        return (c - 48);

    switch (c)
    {
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
        default:
            return -1;
    }

    return -1;
}

static int str2Hex(const char *str, int len)
{
    if (!str || len != 2) return -1;

    int ret = 0x00;

    /* hex[0]: high bit; hex[1] low bit */
    int hex[2];
    if ((hex[0] = char2Int(str[0])) == -1) return -1;
    if ((hex[1] = char2Int(str[1])) == -1) return -1;

    ret |= (hex[0] << 4);
    ret |= hex[1];

    return ret;
}

static int g_can_fd = -1;
int init_can()
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    g_can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (g_can_fd < 0) {
        perror("socket");
        return -1;
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, "can0");
    if (ioctl(g_can_fd, SIOCGIFINDEX, &ifr)) {
        close(g_can_fd);
        perror("ioctl error");
        return -1;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(g_can_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(g_can_fd);
        perror("bind");
        return -1;
    }

    return g_can_fd;
}

int send_msg(const struct can_frame *frame)
{
    int ret = write(g_can_fd, frame, sizeof(*frame));
    printf("write bytes = %d\n", ret);
    if (ret == -1) {
        perror("write failed");
    }

    return ret;
}

int recv_msg(struct can_frame *frame)
{
    int ret = read(g_can_fd, frame, sizeof(struct can_frame));
    if (ret == -1)
        perror("read");

    return ret;
}

int parse_frame(const can_frame *frame,
        uint8_t &priority, uint8_t &s_id, uint8_t &d_id,
        uint8_t *data, uint8_t &len)
{
    if (!(frame->can_id & CAN_EFF_FLAG))
    {
        printf("frame is not extended frame");
        return -1;
    }

    uint32_t extid = frame->can_id & CAN_EFF_MASK;

    /* parse frame id */
    priority = CAN_ID2DATA_PRIORITY(extid); /* priority       */
    s_id     = CAN_ID2DATA_S_MODULE(extid); /* src module id  */
    d_id     = CAN_ID2DATA_D_MODULE(extid);
    len      = frame->can_dlc;

    for (int i = 0; i < len; ++i)
        data[i] = frame->data[i];

    return 0;
}

int construct_frame(struct can_frame *frame,
        uint8_t priority, uint8_t s_id, uint8_t d_id,
        uint8_t *data, uint8_t len)
{
    frame->can_id = CAN_DATA2ID_PRIORITY(priority)
        | CAN_DATA2ID_S_SYS(s_id)
        | CAN_DATA2ID_D_SYS(d_id)
        | CAN_DATA2ID_S_SMOD(s_id)
        | CAN_DATA2ID_D_SMOD(d_id);

    printf("id: 0x%02X\n", frame->can_id);

    frame->can_id &= CAN_EFF_MASK;
    frame->can_id |= CAN_EFF_FLAG;

    int i = 0;
    for (i = 0; i < len; i++)
    {
        frame->data[i] = data[i];
    }
    frame->can_dlc = len;

    printf("data: ");
    for (i = 0; i < len; ++i)
        printf("%02X", frame->data[i]);
    printf("\n");

    return 0;
}

int deinit_can()
{
    sleep(1);
    close(g_can_fd);
    g_can_fd = -1;

    return 0;
}

int send_main(int argc, char **argv)
{
    if (argc != 6)
    {
        printf("arg error!\n");
        printf("Usage: cmd <priority> <s_id> <d_id> <data> <len>\n");
        printf("Notice: all arg is HEX format\n");

        return 0;
    }

    int priority = str2Hex(argv[1], strlen(argv[1]));
    int s_id = str2Hex(argv[2], strlen(argv[2]));
    int d_id = str2Hex(argv[3], strlen(argv[3]));
    int len = str2Hex(argv[5], strlen(argv[5]));

    if (priority == -1 || s_id == -1 || d_id == -1 || len == -1)
    {
        printf("arg error, use HEX format\n");
        return -1;
    }

    uint8_t data[8];
    int i = 0;
    for (i = 0; i < len; ++i)
    {
        data[i] = str2Hex(argv[4]+i*2, 2);
    }

    struct can_frame frame;
    construct_frame(&frame, priority, s_id, d_id, data, len);
    send_msg(&frame);

    return 0;
}

static bool is_dump_running = true;
int dump_main(int argc, char **argv)
{
    struct can_frame frame;
    struct can_packet tmp_packet;
    while (is_dump_running)
    {
        memset(&frame, 0, sizeof(struct can_frame));
        memset(&tmp_packet, 0, sizeof(struct can_packet));

        if (recv_msg(&frame) < 0) return -1;

        parse_frame(&frame, tmp_packet.priority,
                tmp_packet.s_id, tmp_packet.d_id, tmp_packet.data, tmp_packet.len);

        printf("0x%02X->0x%02X, p: 0x%02X, len: 0x%02X, data:",
                tmp_packet.s_id, tmp_packet.d_id,
                tmp_packet.priority, tmp_packet.len);
        for (int i = 0; i < tmp_packet.len; ++i)
            printf(" %02X", tmp_packet.data[i]);
        printf("\n");
    }

    return 0;

}

//cansend,candump
int main(int argc, char **argv)
{
    int ret = -1;
    char *name = argv[0];

    //eg: canutils @cansend
    if((argc > 1) && (argv[1][0] == '@'))
    {
        name = argv[1] + 1;
        argc--;
        argv++;
    }
    else
    {
        char *cmd = strrchr(argv[0], '/');
        if (cmd)
            name = cmd + 1;
    }

    init_can();

    if (!strcmp("cansend", name))
        ret = send_main(argc, argv);
    else if (!strcmp("candump", name))
        ret = dump_main(argc, argv);
    else
        printf("%s: no such cmd\n", argv[0]);

    deinit_can();

    return ret;
}
