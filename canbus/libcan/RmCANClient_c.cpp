/*
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include "RmCANClient.h"
#include "rm_can_data_type.h"
#include "rm_can_log.h"

#include "RmCANClient_c.h"

#undef LOG_TAG
#define LOG_TAG "CAN_CLIENT_C"

using namespace android;

class CANPacketReceiver;

sp<RmCANClient> g_can_client = NULL;
sp<CANPacketReceiver> g_recevier = NULL;
struct can_client_callback g_callback;

/**************** can msg reciver **************/
class CANPacketReceiver : public RmCANReceiver
{
    public:
        void RmRecvCANData(int priority, int src_id, const void *pdata, int len)
        {
            if (g_callback.RmRecvCANData != NULL)
                g_callback.RmRecvCANData(priority, src_id, pdata, len);
        }

        void RmCANServiceDied()
        {
            if (g_callback.RmCANServiceDied != NULL)
                g_callback.RmCANServiceDied();
            LOGE(LOG_TAG, "CAN Service is dead");
        }
};

int RmInitCANClient(int module_id, struct can_client_callback *callback)
{
    if (callback == NULL
            || callback->RmRecvCANData == NULL
            || callback->RmCANServiceDied == NULL)
        return -1;

    RmDeinitCANClient();

    memcpy(&g_callback, callback, sizeof(struct can_client_callback));

    g_can_client = new RmCANClient();
    g_recevier = new CANPacketReceiver();

    g_can_client->RmSetReceiver(module_id, g_recevier);
    return 0;
}

void RmDeinitCANClient(void)
{
    if (g_can_client != NULL)
    {
        g_can_client->disconnect();
        g_can_client.clear();
        g_can_client == NULL;
    }

    if (g_recevier != NULL)
        g_recevier.clear();

    memset(&g_callback, 0, sizeof(struct can_client_callback));
}

int RmSendCANData(int dest_id, const void *pdata, int len, int priority)
{
    int ret = -1;
    if (g_can_client != NULL)
        ret = g_can_client->RmSendCANData(dest_id, pdata, len, priority);

    return ret;
}
