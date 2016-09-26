/*
*/

#include <stdint.h>
#include <sys/types.h>

#include <binder/Parcel.h>
#include <utils/String8.h>

#include "IRmCAN.h"

namespace android
{

enum
{
    RM_DISCONNECT = IBinder::FIRST_CALL_TRANSACTION,
    RM_SEND_CAN_DATA,
};

class BpRmCAN: public BpInterface<IRmCAN>
{
public:
    BpRmCAN(const sp<IBinder>& impl)
        : BpInterface<IRmCAN>(impl)
    {
    }

    /* disconnect from can service */
    virtual void disconnect()
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRmCAN::getInterfaceDescriptor());

        remote()->transact(RM_DISCONNECT, data, &reply);
    }

    virtual int RmSendCANData(int priority, int src_id, int dest_id,
                              const void *pdata, int len)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRmCAN::getInterfaceDescriptor());
        data.writeInt32(priority);
        data.writeInt32(src_id);
        data.writeInt32(dest_id);
        data.writeInt32(len);
        data.write(pdata, len);

        remote()->transact(RM_SEND_CAN_DATA, data, &reply);
        return reply.readInt32();
    }

};

IMPLEMENT_META_INTERFACE(RmCAN, "rm.can.IRmCAN");

/**********************************************************************/

status_t BnRmCAN::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case RM_DISCONNECT:
        {
            CHECK_INTERFACE(IRmCAN, data, reply);

            disconnect();
            return NO_ERROR;
        } break;

        case RM_SEND_CAN_DATA:
        {
            CHECK_INTERFACE(IRmCAN, data, reply);

            int priority = data.readInt32();
            int src_id = data.readInt32();
            int dest_id = data.readInt32();
            int len = data.readInt32();
            const void *pdata = data.readInplace(len);

            reply->writeInt32(RmSendCANData(priority, src_id, dest_id, pdata, len));
            return NO_ERROR;
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

/***************************************************************************/

}; /* namespace android */
