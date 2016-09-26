#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define BUF_SIZ	(255)

int main(int argc, char **argv)
{
    struct can_frame frame;
    struct ifreq ifr;
    struct sockaddr_can addr;
    int nbytes, i, s;
    int running = 1;

    signal(SIGPIPE, SIG_IGN);

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("socket");
        return -1;
    }

    addr.can_family = AF_CAN;
    strncpy(ifr.ifr_name, "can0", sizeof(ifr.ifr_name));

    if (ioctl(s, SIOCGIFINDEX, &ifr)) {
        perror("ioctl");
        return -1;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    while (running) {
        memset(&frame, 0, sizeof(struct can_frame));
        if ((nbytes = read(s, &frame, sizeof(struct can_frame))) < 0) {
            perror("read");
            return -1;
        }
        else if (nbytes > 0)
        {
            if (frame.can_id & CAN_EFF_FLAG)
            {
                printf("id: 0x%x\n", frame.can_id & CAN_EFF_MASK);
            }
            else
            {
                printf("recv error frame\n");
                return -1;
            }

            for (i = 0; i < frame.can_dlc; i++)
            {
                printf("%02x ", frame.data[i]);
            }
            printf("\n");
        }
        else
        {
            printf("read return 0\n");
        }
    }

    return 0;
}
