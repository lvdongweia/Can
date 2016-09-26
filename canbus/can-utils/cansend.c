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

int main(int argc, char **argv)
{
    struct can_frame frame;
    struct ifreq ifr;
    struct sockaddr_can addr;
    int ret = -1;


    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, "can0");
    if (ioctl(s, SIOCGIFINDEX, &ifr)) {
        perror("ioctl");
        return -1;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

#if 0
    //TODO: constructure can frame
    frame.can_id = 0xC400002;
    frame.data[0] = 1;
    frame.data[1] = 3;
    frame.data[2] = 4;

    frame.can_dlc = 3;

    /* use extend can */
    frame.can_id &= CAN_EFF_MASK;
    frame.can_id |= CAN_EFF_FLAG;

    while(1)
    {
        ret = write(s, &frame, sizeof(frame));
        printf("write bytes = %d\n", ret);
        if (ret == -1) {
            perror("write");
        }
        sleep(3);
    }
#else
    frame.can_id = 0xc000303;
    frame.can_id &= CAN_EFF_MASK;
    frame.can_id |= CAN_EFF_FLAG;

    int i = 0;
    for (i = 0; i < 100; i++)
    {
        if (i % 4 == 0)
        {
            frame.data[0] = 0x0;
            frame.data[1] = 0x1;
            frame.data[2] = 0x19;
            frame.data[3] = 0x74;
            frame.data[4] = 0x65;
            frame.data[5] = 0x73;
            frame.data[6] = 0x74;
            frame.data[7] = 0x20;
            frame.can_dlc = 8;
        }
        else if (i % 4 == 1)
        {
            frame.data[0] = 0x1;
            frame.data[1] = 0x64;
            frame.data[2] = 0x65;
            frame.data[3] = 0x62;
            frame.data[4] = 0x75;
            frame.data[5] = 0x67;
            frame.data[6] = 0x2c;
            frame.data[7] = 0x20;
            frame.can_dlc = 8;
        }
        else if (i % 4 == 2)
        {
            frame.data[0] = 0x2;
            frame.data[1] = 0x69;
            frame.data[2] = 0x64;
            frame.data[3] = 0x20;
            frame.data[4] = 0x3d;
            frame.data[5] = 0x20;
            frame.data[6] = 0x39;
            frame.data[7] = 0x39;
            frame.can_dlc = 8;
        }
        else if (i % 4 == 3)
        {
            frame.data[0] = 0x3;
            frame.data[1] = 0x39;
            frame.data[2] = 0x39;
            frame.data[3] = 0x31;
            frame.data[4] = 0x23;
            frame.can_dlc = 5;
        }
        ret = write(s, &frame, sizeof(frame));
        printf("write bytes = %d\n", ret);
        if (ret == -1) {
            perror("write");
        }
    }
#endif
    while(1)
        sleep(1);
    close(s);
    return 0;
}
