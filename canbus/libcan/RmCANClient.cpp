/*
 */

#undef LOG_TAG
#define LOG_TAG "RM_CLIENT"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <utils/String8.h>

#include "IRmCANService.h"
#include "RmCANClient.h"
#include "IRmCAN.h"
#include "rm_can_log.h"

namespace android
{

RmCANClient::RmCANClient()
{
    LOGV(LOG_TAG, "RmCANClient constructor #########################");

    mReceiver = NULL;
    mModuleId = -1;
    mIsRegistered = false;
}

RmCANClient::~RmCANClient()
{
    LOGV(LOG_TAG, "RmCANClient destructor #########################");
    disconnect();
    IPCThreadState::self()->flushCommands();
}

void RmCANClient::disconnect()
{
    LOGV(LOG_TAG, "RmCANClient::disconnect()");
    sp<IRmCAN> p;
    {
        Mutex::Autolock _l(mLock);
        p = mRmCAN;
        mRmCAN.clear();
    }

    if (p != 0)
    {
        p->disconnect();
    }
}

int RmCANClient::RmRegisterClient(int module_id)
{
    const sp<IRmCANService>& service(RmGetCANService());

    if (service != 0)
    {
        sp<IRmCAN> p = mRmCAN;
        mRmCAN = service->RmRegisterClient(getpid(), module_id, this);

        if (p != 0)
        {
            p->disconnect();
        }

        if (mRmCAN != 0)
        {
            mIsRegistered = true;
            return 0;
        }
        else
        {
            mIsRegistered = false;
            LOGE(LOG_TAG, "register client failed");
            return -1;
        }
    }

    LOGE(LOG_TAG, "RmRegisterClient: Unable to locate CAN service");
    return -1;
}

int RmCANClient::RmSetReceiver(int module_id, const sp<RmCANReceiver>& receiver)
{
    Mutex::Autolock _l(mLock);
    mModuleId = module_id;
    mReceiver = receiver;

    int ret = RmRegisterClient(mModuleId);

    /* add the sentence here to prevent forgetting it on application */
    ProcessState::self()->startThreadPool();
    return ret;
}

int RmCANClient::RmSendCANData(int dest_id, const void *pdata,
                               int len, int priority)
{
    int ret = 0;
    Mutex::Autolock _l(mLock);
    ret = RmSendCANData(priority, mModuleId, dest_id, pdata, len);
    return ret;
}

int RmCANClient::RmSendCANData(int priority, int src_id, int dest_id,
        const void *pdata, int len)
{
    int ret = 0;
    if (mRmCAN != 0)
    {
        ret = mRmCAN->RmSendCANData(priority, src_id, dest_id, pdata, len);
    }
    else
    {
        LOGE(LOG_TAG, "RmSendCANData: Unable to locate CAN service");
        ret = -1;
    }

    return ret;
}

void RmCANClient::RmRecvCANData(int priority, int src_id,
                                const void *pdata, int len)
{
    LOGV(LOG_TAG, "message received priority=%d, src_id=0x%x", priority, src_id);

    sp<RmCANReceiver> receiver = mReceiver;

    /* this prevents re-entrant calls into client code */
    if (receiver != 0)
    {
        Mutex::Autolock _l(mRecvLock);
        receiver->RmRecvCANData(priority, src_id, pdata, len);
    }
}

void RmCANClient::died()
{
    LOGE(LOG_TAG, "RmCANClient: CAN service died");
    sp<RmCANReceiver> receiver = mReceiver;

    if (receiver != 0)
    {
        Mutex::Autolock _l(mRecvLock);
        receiver->RmCANServiceDied();
    }
}

}; /* namespace android */
