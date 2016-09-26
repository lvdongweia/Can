/*
 */

#ifndef RM_CAN_CLIENT_H_
#define RM_CAN_CLIENT_H_

#include "IRmCANDeathNotifier.h"
#include "IRmCANClient.h"
#include "rm_can_data_type.h"

namespace android
{

class IRmCAN;

class RmCANReceiver: virtual public RefBase
{
public:
    virtual void RmRecvCANData(
            int priority, int src_id,
            const void *pdata, int len) = 0;
    virtual void RmCANServiceDied() = 0;
};

class RmCANClient : public BnRmCANClient,
                    public virtual IRmCANDeathNotifier
{
public:
    RmCANClient();
    ~RmCANClient();
    int             RmSetReceiver(int module_id, const sp<RmCANReceiver>& receiver);
    int             RmSendCANData(int dest_id, const void *pdata,
                                  int len, int priority = ROBOT_CAN_LEVEL3);
    void            disconnect();

    /**************************** binder service callback **********************************/

    virtual void    RmRecvCANData(int priority, int src_id,
                                  const void *pdata, int len);
    virtual void    died();

private:
    int             RmRegisterClient(int module_id);
    int             RmSendCANData(int priority, int src_id, int dest_id,
                                  const void *pdata, int len);

    Mutex             mLock;
    Mutex             mRecvLock;
    bool              mIsRegistered;
    int               mModuleId;
    sp<IRmCAN>        mRmCAN;
    sp<RmCANReceiver> mReceiver;
};

}; /* namespace android */

#endif /* RM_CAN_CLIENT_H_ */
