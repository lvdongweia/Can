#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

#include "RmCANClient.h"
#include "rm_can_data_type.h"
#include "rm_can_log.h"

#undef LOG_TAG
#define LOG_TAG "TEST_CAN"

using namespace android;
int64_t num = 0;
/**************** can msg reciver **************/
class CANPacketReceiver : public RmCANReceiver
{
public:
    void RmRecvCANData(int priority, int src_id, const void *pdata, int len)
    {
        num++;
        //LOGI(LOG_TAG, "%lld", num);
        //LOGD(LOG_TAG, "pid: %d, priority: %d, src_id: %d, data: %s, len: %d, total = %ld",
        //        getpid(), priority, src_id, (char*)pdata, len, num);
    }

    void RmCANServiceDied()
    {
        LOGE(LOG_TAG, "CAN Service is dead");
    }
};

int test_can_main()
{
    char *msg_data = "hello world";

    sp<RmCANClient> canClient = new RmCANClient();
    sp<CANPacketReceiver> recevier = new CANPacketReceiver();

    canClient->RmSetReceiver(RM_SYSCTRL_ID, recevier);

    int64_t counts = 1000000;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    while(counts > 0)
    {

        //asprintf(&msg_data, "hello world, id = %d", counts);
        canClient->RmSendCANData(RM_SYSCTRL_ID, msg_data, strlen(msg_data) + 1);

        //free(msg_data);
        //msg_data = NULL;

        counts--;
        usleep(1 * 500);
    }
    LOGI(LOG_TAG, "%lld", num);
    gettimeofday(&tv, NULL);
    uint64_t last = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    LOGI(LOG_TAG, "############################ %llu", last - now);

    int i = 30;
    while (i > 0)
    {
        LOGI(LOG_TAG, "not complete");
        i--;
        sleep(1);
    }

    canClient->disconnect();
    canClient.clear();
    recevier.clear();

    LOGD(LOG_TAG, "finish test can");
    sleep(10);

    return 0;
}

int test_send_debug()
{
    char *log_info = NULL;
    void *msg_data = NULL;

    sp<RmCANClient> canClient = new RmCANClient();
    sp<CANPacketReceiver> recevier = new CANPacketReceiver();

    canClient->RmSetReceiver(RM_SYSCTRL_ID, recevier);
    int counts = 100000;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    while(counts > 0)
    {
        /* construct pdata */

        asprintf(&log_info, "test debug, id = %d#", counts);

        int16_t length = 1 + 1 + strlen(log_info); /* level + length + info_len */
        msg_data = malloc(length);
        memset(msg_data, 0, length);

        int8_t level;
        if (counts % 3 == 0)
            level = ROBOT_LOG_DEBUG;
        else if (counts % 3 == 1)
            level = ROBOT_LOG_INFO;
        else if (counts % 3 == 2)
            level = ROBOT_LOG_WARN;

        memcpy(msg_data, &level, 1);
        memcpy((char*)msg_data + 1, &length, 1);
        memcpy((char*)msg_data + 2, log_info, strlen(log_info));

        //canClient->RmSendCANData(RM_DEBUG_ID, msg_data, length);
        canClient->RmSendCANData(ROBOT_RADIOCAST_ID, msg_data, length);

        free(log_info);
        free(msg_data);

        counts--;
        sleep(1);
    }

    gettimeofday(&tv, NULL);
    uint64_t last = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    LOGI(LOG_TAG, "############################ %llu", last - now);

    canClient->disconnect();
    canClient.clear();
    recevier.clear();

    LOGD(LOG_TAG, "finish test send debug");
    sleep(10);

    return 0;
}

int test_send_fault()
{
    void *msg_data = NULL;

    sp<RmCANClient> canClient = new RmCANClient();
    sp<CANPacketReceiver> recevier = new CANPacketReceiver();

    canClient->RmSetReceiver(RM_SYSCTRL_ID, recevier);
    int counts = 100000;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    while(counts > 0)
    {
        /* construct pdata */

        int8_t length = 1 + 1 + 1; /* level + length + id */
        msg_data = malloc(length);
        memset(msg_data, 0, length);

        int8_t level;
        if (counts % 3 == 0)
            level = ROBOT_FAULT_WARN;
        else if (counts % 3 == 1)
            level = ROBOT_FAULT_ERROR;
        else if (counts % 3 == 2)
            level = ROBOT_FAULT_FATAL;

        int8_t fault_id = 1;

        memcpy(msg_data, &level, 1);
        memcpy((char*)msg_data + 1, &length, 1);
        memcpy((char*)msg_data + 2, &fault_id, 1);

        canClient->RmSendCANData(RM_FAULT_ID, msg_data, length);

        free(msg_data);

        counts--;
        sleep(3);
    }

    gettimeofday(&tv, NULL);
    uint64_t last = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    LOGI(LOG_TAG, "############################ %llu", last - now);

    canClient->disconnect();
    canClient.clear();
    recevier.clear();

    LOGD(LOG_TAG, "finish test send fault");
    sleep(10);

    return 0;
}
int main(int argc, char **argv)
{
    int ret = 0;

    //ret = test_can_main();
    ret = test_send_debug();
    //ret = test_send_fault();

    return ret;
}
