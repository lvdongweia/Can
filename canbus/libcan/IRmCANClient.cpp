/*
 */

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

#include "IRmCANClient.h"
#include "rm_can_log.h"

#undef LOG_TAG
#define LOG_TAG "IRM_CAN_CLIENT"

namespace android
{

enum
{
    RM_RECV_CAN_DATA = IBinder::FIRST_CALL_TRANSACTION,
};

class BpRmCANClient: public BpInterface<IRmCANClient>
{
public:
    BpRmCANClient(const sp<IBinder>& impl)
        : BpInterface<IRmCANClient>(impl)
    {
    }

    virtual void RmRecvCANData(int priority, int src_id,
                              const void *pdata, int len)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRmCANClient::getInterfaceDescriptor());
        data.writeInt32(priority);
        data.writeInt32(src_id);
        data.writeInt32(len);
        data.write(pdata, len);

        remote()->transact(RM_RECV_CAN_DATA, data, &reply, IBinder::FLAG_ONEWAY);
    }
};

IMPLEMENT_META_INTERFACE(RmCANClient, "rm.can.IRmCANClient");

/****************************************************************************/

status_t BnRmCANClient::onTransact(
    uint32_t code, const Parcel& data,
    Parcel* reply, uint32_t flags)
{
    switch (code)
    {
        case RM_RECV_CAN_DATA:
        {
            CHECK_INTERFACE(IRmCANClient, data, reply);

            int priority = data.readInt32();
            int src_id = data.readInt32();
            int len = data.readInt32();
            const void *pdata = data.readInplace(len);

            RmRecvCANData(priority, src_id, pdata, len);
            return NO_ERROR;
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}; /* namespace android */
