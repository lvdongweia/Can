/*
 */

#ifndef RM_CAN_ICAN_CLIENT_H_
#define RM_CAN_ICAN_CLIENT_H_

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

namespace android
{

class IRmCANClient: public IInterface
{
public:
    DECLARE_META_INTERFACE(RmCANClient);

    virtual void RmRecvCANData(int priority,
                               int src_id,
                               const void *pdata,
                               int len) = 0;
};

/* ---------------------------------------------------------------------------- */

class BnRmCANClient: public BnInterface<IRmCANClient>
{
public:
    virtual status_t    onTransact(uint32_t code,
                                   const Parcel& data,
                                   Parcel* reply,
                                   uint32_t flags = 0);
};

}; /* namespace android */

#endif /* RM_CAN_ICAN_CLIENT_H_ */
