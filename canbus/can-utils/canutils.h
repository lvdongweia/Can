#ifndef __CAN_UTILS_H_
#define __CAN_UTILS_H_

#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int init_can();

int send_msg(const struct can_frame *frame);
int recv_msg(struct can_frame *frame);

int parse_frame(const can_frame *frame,
        uint8_t &priority, uint8_t &s_id, uint8_t &d_id,
        uint8_t *data, uint8_t &len);
int construct_frame(struct can_frame *frame,
        uint8_t priority, uint8_t s_id, uint8_t d_id,
        uint8_t *data, uint8_t len);

int deinit_can();

#endif /* __CAN_UTILS_H_ */
