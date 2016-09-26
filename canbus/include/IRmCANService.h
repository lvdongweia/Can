/*
 */

#ifndef RM_CAN_ICAN_SERVICE_H_
#define RM_CAN_ICAN_SERVICE_H_

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

namespace android {

class IRmCANClient;
class IRmCAN;

class IRmCANService : public IInterface
{
public:
    DECLARE_META_INTERFACE(RmCANService);

    virtual sp<IRmCAN>  RmRegisterClient(pid_t pid, int module_id,
                                         const sp<IRmCANClient>& client) = 0;

};

// ----------------------------------------------------------------------------

class BnRmCANService : public BnInterface<IRmCANService>
{
public:
    virtual status_t    onTransact(uint32_t code,
                                   const Parcel& data,
                                   Parcel* reply,
                                   uint32_t flags = 0);
};

}; /* namespace android */

#endif /* RM_CAN_ICAN_SERVICE_H_ */
