/*
 */

/* Proxy for CAN implementations */

#define LOG_TAG "RM_CAN_SERVICE"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#include "RmCANThread.h"
#include "rm_can_log.h"

#include "IRmCANClient.h"
#include "RmCANService.h"
#include "RmCANDevice.h"

namespace android
{

void RmCANService::instantiate()
{
    defaultServiceManager()->addService(
            String16("rm.can.service"), new RmCANService());
}

RmCANService::RmCANService() : mTimeOutId(127)
{
    LOGV(LOG_TAG, "RmCANService created ###################");

    for (int i = 0; i < RM_MODULE_NUM; i++)
        mClients[i] = NULL;

    memset(mPacketsId, 0, 16);

    mCANDevice = new RmCANDevice();
    if (!mCANDevice->isOpened())
    {
        LOGE(LOG_TAG, "open can device failed");
        //TODO: ???????
    }

    RmCANStart();
}

RmCANService::~RmCANService()
{
    LOGV(LOG_TAG, "RmCANService destroyed #######################");

    RmCANStop();

    for (int i = 0; i < RM_MODULE_NUM; i++)
    {
        if (mClients[i] != NULL)
            mClients[i].clear();
    }

    RmReleaseCANData(mSendList);
    RmReleaseCANData(mRecvList);

    /*
     * maybe error, release sequence:
     * Vector.clear
     * KeyedVector.clear
     * KeyedVector.clear
     */
    mInCompletePackets.clear();

    delete mCANDevice;
}

void RmCANService::RmReleaseCANData(List<struct RmCANPacket*>& list)
{
    if (!list.empty())
    {
        List<struct RmCANPacket*>::iterator iter = list.begin();
        for (; iter != list.end(); )
        {
            if ((*iter)->p_data != NULL) delete (*iter);
            iter = list.erase(iter);
        }
    }
}

void RmCANService::RmCANStart()
{
    mTransThread = new RmCANTransThread(this, mCANDevice->RmGetDeviceFd());
    mRecvThread  = new RmCANReceiverThread(this);

    if (mTransThread.get() != NULL)
        mTransThread->run("CAN Trans thread start");

    if (mRecvThread.get() != NULL)
        mRecvThread->run("Recv CAN thread start");
}

int RmCANService::RmReInitCANDevice(int *sock)
{
    int ret = -1;
    if (mCANDevice != NULL)
    {
        ret = mCANDevice->RmInitCANDevice();
        *sock = mCANDevice->RmGetDeviceFd();
    }

    return ret;
}

void RmCANService::RmCANStop()
{
    if (mTransThread.get() != NULL)
    {
        mTransThread->requestExitAndWait();
        mTransThread.clear();
    }

    if (mRecvThread.get() != NULL)
    {
        mRecvThread->requestExitAndWait();
        mRecvThread.clear();
    }
}

sp<IRmCAN> RmCANService::RmRegisterClient(pid_t pid, int module_id, const sp<IRmCANClient>& client)
{
    LOGV(LOG_TAG, "register new client: module id: %d", module_id);

    if (!isRmVaildModule(module_id))
    {
        LOGE(LOG_TAG, "register failed: module is not RM_XXX");
        return NULL;
    }

    sp<Client> c = new Client(this, pid, client, module_id);

    int index = RmGetModuleIndex(module_id);

    sp<Client> old;
    {
        Mutex::Autolock lock(mLock);
        if (mClients[index] != 0)
        {
            old = mClients[index];
            mClients[index].clear();
        }
        mClients[index] = c;
    }

    if (old != 0) old.clear();

    return c;
}

bool RmCANService::isRmVaildModule(int module_id)
{
    bool ret = false;
    ret = ((module_id >= 0) && !(module_id & 0xf0)
            && (module_id & 0x0f) < RM_MODULE_NUM);
    return ret;
}

RmCANService::RmCANPacket *RmCANService::RmGetCANPacket(
        int priority, int src_id, int dest_id,
        const void *pdata, int len)
{
    RmCANPacket *packet = NULL;

    void *ptr = malloc(len);
    if (!ptr)
    {
        LOGF(LOG_TAG, "malloc request failed: %s", strerror(errno));
        return NULL;
    }

    memcpy(ptr, pdata, len);

    packet = new RmCANPacket;
    packet->priority = priority;
    packet->src_id   = src_id;
    packet->dest_id  = dest_id;
    packet->p_data   = ptr;
    packet->len      = len;

    return packet;
}

RmCANService::RmCANPacket *RmCANService::RmGetCANPacket(
        const RmCANService::RmCANPacket *packet)
{
    RmCANPacket *packet1 = RmGetCANPacket(
            packet->priority,
            packet->src_id,
            packet->dest_id,
            packet->p_data,
            packet->len);

    return packet1;
}

/* can data from client */
int RmCANService::RmSendCANData(int priority, int src_id, int dest_id,
                                const void *pdata, int len)
{
    LOGV(LOG_TAG, "RmSendCANData: priority: %d, src id: 0x%x, dest id: 0x%x, len: %d",
            priority, src_id, dest_id, len);

    if (!pdata) return -1;

    RmCANPacket *packet = RmGetCANPacket(priority, src_id, dest_id, pdata, len);
    if (packet == NULL) return -1;

    if (isRmVaildModule(dest_id))
    {
        /* Rm_module1 -> Rm_module2 */
        RmPushToRecvList(packet);
    }
#if !RUNNING_IN_SIMULATOR
    else if (!isBroadcast(dest_id))
    {
        RmPushToSendList(packet);
    }
    else
    {
        RmHandleBroadcast(packet);
    }
#endif
    return 0;
}

int RmCANService::RmHandleBroadcast(RmCANPacket *packet)
{
    RmCANPacket *packet1 = RmGetCANPacket(packet);

    RmPushToSendList(packet);
    if (packet1 != NULL)
    {
        RmPushToRecvList(packet1);
        return 0;
    }

    return -1;
}

void RmCANService::RmPushToRecvList(RmCANPacket *packet)
{
    Mutex::Autolock lock(mRecvLock);
    mRecvList.push_back(packet);
    mRecvSignal.signal();
}

void RmCANService::RmPushToSendList(RmCANPacket *packet)
{
    Mutex::Autolock lock(mSendLock);
    mSendList.push_back(packet);
}

bool RmCANService::isBroadcast(uint8_t dest_id)
{
    return (dest_id == ROBOT_RADIOCAST_ID);
}

bool RmCANService::isSendListEmpty()
{
    Mutex::Autolock lock(mSendLock);
    return mSendList.empty();
}

/* handle send list data */
int RmCANService::RmSendToCAN()
{
    RmCANPacket *packet = NULL;
    {
        Mutex::Autolock lock(mSendLock);
        if (mSendList.empty()) return -1;

        List<struct RmCANPacket*>::iterator iter = mSendList.begin();
        packet = *iter;
        mSendList.erase(iter);

    }

    LOGV(LOG_TAG, "RmSendToCAN: priority: %d, src_id: 0x%x, dest_id: 0x%x",
            packet->priority, packet->src_id, packet->dest_id);

    int ret = -1;
    struct flexcan_packet can_data;
    memset(&can_data, 0, sizeof(struct flexcan_packet));

    /* common data */
    can_data.priority = packet->priority;
    can_data.s_id     = packet->src_id;
    can_data.d_id     = packet->dest_id;

    if (packet->len <= CAN_DATA_MAX_LEN)
    {
        can_data.len  = packet->len;
        memcpy(can_data.data, packet->p_data, packet->len);
        ret = mCANDevice->RmCANSend(&can_data);

        /*
         * memset:
         * can_data.packet_id = 0;
         * can_data.packet_SN = 0;
         * can_data.sum       = 0;
         */
    }
    else
    {
        int index = RmGetModuleIndex(packet->src_id);

        can_data.packet_id = mPacketsId[index];
        mPacketsId[index]++;

        uint8_t num = (packet->len % CAN_SUB_PACKET_SIZE == 0)
            ? (packet->len / CAN_SUB_PACKET_SIZE)
            : (packet->len / CAN_SUB_PACKET_SIZE + 1);
        can_data.sum = num - 1;                                /* 0 ~ num - 1 */

        uint8_t *tmp_p = (uint8_t*)(packet->p_data);
        can_data.len = CAN_SUB_PACKET_SIZE;
        /* 0 ~ num-2 */
        for(int i = 0; i < num - 1; ++i)
        {
            can_data.packet_SN = i;
            memcpy(can_data.data, tmp_p, CAN_SUB_PACKET_SIZE);

            ret = mCANDevice->RmCANSend(&can_data);
            if (ret != 0) goto send_error;
            usleep(2 * 1000);

            tmp_p += CAN_SUB_PACKET_SIZE;
            memset(can_data.data, 0, CAN_DATA_MAX_LEN);
        }

        /* send lastest sub-packet */
        uint32_t nleft = packet->len - (num - 1) * CAN_SUB_PACKET_SIZE;
        can_data.len = nleft;
        can_data.packet_SN = num - 1;
        memcpy(can_data.data, tmp_p, nleft);

        ret = mCANDevice->RmCANSend(&can_data);
    }

send_error:
    if (ret != 0) LOGE(LOG_TAG, "send data to can driver failed: %s", strerror(errno));

    delete packet;

    return ret;
}

/* send recv_list data to client process */
int RmCANService::RmRecvCANData(int priority, int src_id, int dest_id,
                                const void *pdata, int len)
{
    LOGV(LOG_TAG, "RmRecvCANData: priority: %d, src_id: 0x%x, dest_id: 0x%x",
            priority, src_id, dest_id);

    /* lock mClients */
    Mutex::Autolock lock(mLock);
    if (isRmVaildModule(dest_id))
    {
        int index = RmGetModuleIndex(dest_id);

        if (mClients[index] != NULL)
        {
            mClients[index]->RmRecvCANData(priority, src_id, pdata, len);
        }
    }
    else if (isBroadcast(dest_id))
    {
        uint8_t broadcastReceiverIndex = RmGetModuleIndex(dest_id);
        uint8_t srcIndex = RmGetModuleIndex(src_id);
        /* from: RM_ sub-system */
        if (isRmVaildModule(src_id)
                && srcIndex == broadcastReceiverIndex)
            return -1;

        if (mClients[broadcastReceiverIndex] != NULL)
            mClients[broadcastReceiverIndex]->RmRecvCANData(priority, src_id, pdata, len);
    }
    else
        return -1;

    return 0;
}

/* handle recv list data */
int RmCANService::RmRecvFromList()
{
    RmCANPacket *packet = NULL;
    {
        Mutex::Autolock lock(mRecvLock);
        if (mRecvList.empty()) mRecvSignal.wait(mRecvLock);

        List<struct RmCANPacket*>::iterator iter = mRecvList.begin();
        packet = *iter;
        mRecvList.erase(iter);
    }

    RmRecvCANData(packet->priority, packet->src_id, packet->dest_id,
                  packet->p_data, packet->len);

    if (packet != NULL)
    {
        delete packet;
    }

    return 0;
}

/* recv can data from can driver */
int RmCANService::RmRecvFromCAN()
{
    struct flexcan_packet can_data;
    memset(&can_data, 0, sizeof(struct flexcan_packet));

    int ret = mCANDevice->RmCANRecv(&can_data);
    if (ret < 0) return -1;

    RmCANPacket *packet = NULL;
    /* incomplete packet */
    if (can_data.sum != 0)
    {
        /* need to construct a packet */
        /* <src_id, KeyVector> */
        ssize_t module_index = mInCompletePackets.indexOfKey(can_data.s_id);
        if (module_index >= 0)
        {
            /* <packet_id, Vector> */
            ssize_t packet_index =
                mInCompletePackets.valueAt(module_index).indexOfKey(can_data.packet_id);
            if (packet_index >= 0)
            {
                mInCompletePackets.editValueAt(module_index).editValueAt(packet_index).push_back(can_data);
                size_t num =
                    mInCompletePackets.valueAt(module_index).valueAt(packet_index).size();
                /* sum: 0 ~ num-1 */
                //TODO: packet error handle ??????????????????
                if ((can_data.sum + 1) <= num)
                {
                    packet =
                        RmGetCANPacket(mInCompletePackets.editValueAt(module_index).editValueAt(packet_index));

                    RmRemovePacketId(mInCompletePackets.editValueAt(module_index), packet_index);
                    LOGV(LOG_TAG, "complete packets num = %u", num);
                    goto complete;
                }
            }
            else
            {
                /* delete old packet_id(timeout id)
                 * Note: Not a good solution, May lose data
                 */
                uint8_t old_packet_id = can_data.packet_id + mTimeOutId;
                ssize_t old_packet_index =
                    mInCompletePackets.valueAt(module_index).indexOfKey(old_packet_id);
                if (old_packet_index >= 0)
                {
                    RmRemovePacketId(mInCompletePackets.editValueAt(module_index), old_packet_index);
                }

                /* add new packet_id */
                Vector<struct flexcan_packet> packets;
                packets.push_back(can_data);

                /* KeyVector<packet_id, Vector> */
                mInCompletePackets.editValueAt(module_index).add(can_data.packet_id, packets);
            }
        }
        else
        {
            /* Vector<> */
            Vector<struct flexcan_packet> packets;
            packets.push_back(can_data);

            /* KeyVector<packet_id, Vector> */
            KeyedVector<uint8_t, Vector<struct flexcan_packet> > packets_id;
            packets_id.add(can_data.packet_id, packets);

            /* KeyVector<src_id, KeyVector> */
            mInCompletePackets.add(can_data.s_id, packets_id);
        }

        return 0;
    }

    /* complete packet */
    packet = RmGetCANPacket(&can_data);

complete:
    if (packet != NULL)
    {
        RmPushToRecvList(packet);
    }

    return 0;
}

void RmCANService::RmRemovePacketId(
        KeyedVector<uint8_t, Vector<struct flexcan_packet> > &packets, ssize_t index)
{
    packets.editValueAt(index).clear();
    packets.removeItemsAt(index);
}

static int RmPacketCompare(
        const struct flexcan_packet *lpacket,
        const struct flexcan_packet *rpacket)
{
    return (lpacket->packet_SN > rpacket->packet_SN);
}

/* constructor packet from can_data */
RmCANService::RmCANPacket *RmCANService::RmGetCANPacket(Vector<struct flexcan_packet> packets)
{
    status_t ret = packets.sort(RmPacketCompare);
    if (ret != NO_ERROR)
    {
        LOGE(LOG_TAG, "Vector sort error");
        return NULL;
    }

    uint8_t data[CAN_DATA_TOTAL_SIZE];
    uint32_t len = 0;

    memset(data, 0, CAN_DATA_TOTAL_SIZE);

    Vector<struct flexcan_packet>::const_iterator iter;
    for (iter = packets.begin(); iter != packets.end(); iter++)
    {
        memcpy(data + len, (*iter).data, (*iter).len);
        len += (*iter).len;
    }

    RmCANPacket *packet = RmGetCANPacket(
            packets.top().priority, packets.top().s_id,
            packets.top().d_id, data, len);

    return packet;
}

RmCANService::RmCANPacket *RmCANService::RmGetCANPacket(const struct flexcan_packet *can_data)
{
    RmCANPacket *packet = RmGetCANPacket(
            can_data->priority, can_data->s_id,
            can_data->d_id, can_data->data,
            can_data->len);

    return packet;
}

int RmCANService::RmGetModuleIndex(int module_id)
{
    return (module_id & 0x0f);
}

void RmCANService::RmRemoveClient(int module_id, const Client *client)
{
    LOGV(LOG_TAG, "RmCANService: remove client %d", module_id);
    if (!isRmVaildModule(module_id)) return;

    int index = RmGetModuleIndex(module_id);

    sp<Client> _client;
    {
        Mutex::Autolock lock(mLock);
        if (client != mClients[index].get())
            LOGW(LOG_TAG, "module's id does not match with Client's addr!!!");

        _client = mClients[index];
        mClients[index] = NULL;     /* just reset, client will don't recv can data */
    }

    if (_client.get() != NULL) _client.clear();
}

RmCANService::Client::Client(const sp<RmCANService>& service, pid_t pid,
                             const sp<IRmCANClient>& client, int module_id)
{
    LOGV(LOG_TAG, "Client(pid: %d) constructor, module id: %d ####################", pid, module_id);
    mPid = pid;
    mService = service;
    mClient = client;
    mModuleId = module_id;
}

RmCANService::Client::~Client()
{
    LOGV(LOG_TAG, "Client(pid: %d) destructor module: %d ###################", mPid, mModuleId);
    mClient.clear();
    IPCThreadState::self()->flushCommands();
}

void RmCANService::Client::disconnect()
{
    LOGV(LOG_TAG, "RmCANService::Client::disconnect");
    mService->RmRemoveClient(mModuleId, this);
}

int RmCANService::Client::RmSendCANData(int priority, int src_id, int dest_id,
                                         const void *pdata, int len)
{
    int ret = -1;
    if (mService.get() != NULL)
        ret = mService->RmSendCANData(priority, src_id, dest_id, pdata, len);

    return ret;
}

int RmCANService::Client::RmRecvCANData(int priority, int src_id,
                                        const void *pdata, int len)
{
    int ret = -1;
    if (mClient.get() != NULL)
    {
        mClient->RmRecvCANData(priority, src_id, pdata, len);
        ret = 0;
    }

    return ret;
}

}; /* namespace android */
