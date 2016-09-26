/*
 */

#include <stdint.h>
#include <sys/types.h>

#include "IRmCANClient.h"
#include "IRmCANService.h"
#include "IRmCAN.h"
#include "rm_can_log.h"

#undef LOG_TAG
#define LOG_TAG "IRM_CAN_SERVICE"

namespace android
{

enum
{
    RM_REGISTER_CLIENT = IBinder::FIRST_CALL_TRANSACTION,
};

class BpRmCANService: public BpInterface<IRmCANService>
{
public:
    BpRmCANService(const sp<IBinder>& impl)
        : BpInterface<IRmCANService>(impl)
    {
    }

    virtual sp<IRmCAN> RmRegisterClient(pid_t pid, int module_id, const sp<IRmCANClient>& client)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IRmCANService::getInterfaceDescriptor());
        data.writeInt32(pid);
        data.writeInt32(module_id);
        data.writeStrongBinder(client->asBinder());

        remote()->transact(RM_REGISTER_CLIENT, data, &reply);
        return interface_cast<IRmCAN>(reply.readStrongBinder());
    }

};

IMPLEMENT_META_INTERFACE(RmCANService, "rm.can.IRmCANService");

/*****************************************************************************/

status_t BnRmCANService::onTransact(
    uint32_t code, const Parcel& data,
    Parcel* reply, uint32_t flags)
{
    switch (code)
    {
        case RM_REGISTER_CLIENT:
        {
            CHECK_INTERFACE(IRmCANService, data, reply);

            pid_t pid     = data.readInt32();
            int module_id = data.readInt32();

            sp<IRmCANClient> client =
                interface_cast<IRmCANClient>(data.readStrongBinder());

            sp<IRmCAN> rmcan = RmRegisterClient(pid, module_id, client);

            reply->writeStrongBinder(rmcan->asBinder());
            return NO_ERROR;
        } break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}; /* namespace android */
