/*
 */

#ifndef RM_CAN_ICAN_DEATH_NOTIFIER_H_
#define RM_CAN_ICAN_DEATH_NOTIFIER_H_

#include <utils/threads.h>
#include <utils/RefBase.h>
#include <binder/IBinder.h>
#include <utils/SortedVector.h>

namespace android
{

class IRmCANService;

class IRmCANDeathNotifier: virtual public RefBase
{
public:
    IRmCANDeathNotifier() { RmAddObitRecipient(this); }
    virtual ~IRmCANDeathNotifier() { RmRemoveObitRecipient(this); }

    virtual void died() = 0;
    static const sp<IRmCANService>& RmGetCANService();

private:
    IRmCANDeathNotifier &operator=(const IRmCANDeathNotifier &);
    IRmCANDeathNotifier(const IRmCANDeathNotifier &);

    static void RmAddObitRecipient(const wp<IRmCANDeathNotifier>& recipient);
    static void RmRemoveObitRecipient(const wp<IRmCANDeathNotifier>& recipient);

    class RmDeathNotifier: public IBinder::DeathRecipient
    {
    public:
                RmDeathNotifier() {}
        virtual ~RmDeathNotifier();

        virtual void binderDied(const wp<IBinder>& who);
    };

    friend class RmDeathNotifier;

    static  Mutex                                   sServiceLock;
    static  sp<IRmCANService>                       sRmCANService;
    static  sp<RmDeathNotifier>                     sDeathNotifier;
    static  SortedVector< wp<IRmCANDeathNotifier> > sObitRecipients;
};

}; /* namespace android */

#endif /* RM_CAN_ICAN_DEATH_NOTIFIER_H_ */
