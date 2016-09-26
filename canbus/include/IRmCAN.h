/*
 */

#ifndef RM_ICAN_H_
#define RM_ICAN_H_

#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/KeyedVector.h>

namespace android
{

class Parcel;

class IRmCAN: public IInterface
{
public:
    DECLARE_META_INTERFACE(RmCAN);

    virtual void            disconnect() = 0;
    virtual int             RmSendCANData(int priority, int src_id, int dest_id,
                                          const void *pdata, int len) = 0;
};

/*************************************************************************/

class BnRmCAN: public BnInterface<IRmCAN>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; /* namespace android */

#endif /* RM_ICAN_H_ */
