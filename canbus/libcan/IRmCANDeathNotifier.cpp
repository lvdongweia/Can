/*
*/

#undef LOG_TAG
#define LOG_TAG "IRM_CAN_DEATH_NOTIFIER"

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include "IRmCANService.h"
#include "IRmCANDeathNotifier.h"
#include "rm_can_log.h"

namespace android {

/* client singleton for binder interface to services */
Mutex IRmCANDeathNotifier::sServiceLock;
sp<IRmCANService> IRmCANDeathNotifier::sRmCANService;
sp<IRmCANDeathNotifier::RmDeathNotifier> IRmCANDeathNotifier::sDeathNotifier;
SortedVector< wp<IRmCANDeathNotifier> > IRmCANDeathNotifier::sObitRecipients;

/* establish binder interface to RmCANService, static function */
const sp<IRmCANService>& IRmCANDeathNotifier::RmGetCANService()
{
    Mutex::Autolock _l(sServiceLock);
    if (sRmCANService == 0)
    {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        do
        {
            binder = sm->getService(String16("rm.can.service"));
            if (binder != 0)
            {
                break;
            }

            LOGW(LOG_TAG, "can service not published, waiting...");
            usleep(500000); /* 0.5 s */
        } while (true);

        if (sDeathNotifier == NULL)
        {
            sDeathNotifier = new RmDeathNotifier();
        }

        binder->linkToDeath(sDeathNotifier);
        sRmCANService = interface_cast<IRmCANService>(binder);
    }

    if (sRmCANService == 0) LOGE(LOG_TAG, "no can service!?");
    return sRmCANService;
}

/*static*/
void IRmCANDeathNotifier::RmAddObitRecipient(const wp<IRmCANDeathNotifier>& recipient)
{
    LOGV(LOG_TAG, "RmAddObitRecipient    #######################");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.add(recipient);
}

/*static*/
void IRmCANDeathNotifier::RmRemoveObitRecipient(const wp<IRmCANDeathNotifier>& recipient)
{
    LOGV(LOG_TAG, "RmRemoveObitRecipient ###############################");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.remove(recipient);
}

void IRmCANDeathNotifier::RmDeathNotifier::binderDied(const wp<IBinder>& who)
{
    LOGW(LOG_TAG, "can server died");

    /* Need to do this with the lock held */
    SortedVector< wp<IRmCANDeathNotifier> > list;
    {
        Mutex::Autolock _l(sServiceLock);
        sRmCANService.clear();
        list = sObitRecipients;
    }

    /* Notify application when can server dies.
     * Don't hold the static lock during callback in case app
     * makes a call that needs the lock. */
    size_t count = list.size();
    for (size_t iter = 0; iter < count; ++iter)
    {
        sp<IRmCANDeathNotifier> notifier = list[iter].promote();
        if (notifier != 0) {
            notifier->died();
        }
    }
}

IRmCANDeathNotifier::RmDeathNotifier::~RmDeathNotifier()
{
    LOGV(LOG_TAG, "RmDeathNotifier::~RmDeathNotifier ############################");
    Mutex::Autolock _l(sServiceLock);
    sObitRecipients.clear();
    if (sRmCANService != 0)
    {
        sRmCANService->asBinder()->unlinkToDeath(this);
    }
}

}; /* namespace android */
