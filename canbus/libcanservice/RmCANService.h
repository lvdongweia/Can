/*
*/

#ifndef RM_CAN_SERVICE_H_
#define RM_CAN_SERVICE_H_

#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/List.h>
#include <utils/String8.h>
#include <utils/Vector.h>

#include "rm_can_data_type.h"
#include "IRmCANService.h"
#include "IRmCAN.h"

struct flexcan_packet;
namespace android
{

class IRmCANClient;
class RmCANDevice;

class RmCANService : public BnRmCANService
{
public:
    static  void        instantiate();
    virtual sp<IRmCAN>  RmRegisterClient(pid_t pid, int module_id,
                                         const sp<IRmCANClient>& client);

    /* thread objects called */
    int             RmRecvFromCAN();
    int             RmSendToCAN();
    int             RmRecvFromList();
    bool            isSendListEmpty();
    int             RmReInitCANDevice(int *sock);

private:
    struct RmCANPacket
    {
        uint8_t  priority;
        uint8_t  src_id;
        uint8_t  dest_id;
        void     *p_data;
        uint32_t len;

        RmCANPacket() { p_data = NULL; }
        ~RmCANPacket() { if (p_data) free(p_data); }
    };

    class Client : public BnRmCAN
    {
    public:
        virtual int     RmSendCANData(int priority, int src_id, int dest_id,
                                      const void *pdata, int len);
        virtual void    disconnect();

    private:
        friend class RmCANService;

                              Client(const sp<RmCANService>& service, pid_t pid,
                                     const sp<IRmCANClient>& client, int module_id);
                              Client();
        virtual               ~Client();

        int                   RmRecvCANData(int priority, int src_id,
                                            const void *pdata, int len);

        int                   mModuleId;
        pid_t                 mPid;
        sp<RmCANService>      mService;
        sp<IRmCANClient>      mClient;

    }; /* class Client */

    void                      RmRemoveClient(int module_id, const Client *client);
    int                       RmSendCANData(int priority, int src_id, int dest_id,
                                            const void *pdata, int len);
                              RmCANService();
    virtual                   ~RmCANService();
    void                      RmCANStart();
    void                      RmCANStop();
    bool                      isRmVaildModule(int module_id);
    bool                      isBroadcast(uint8_t dest_id);
    int                       RmGetModuleIndex(int module_id);
    void                      RmReleaseCANData(List<struct RmCANPacket*>& list);
    int                       RmRecvCANData(int priority, int src_id, int dest_id,
                                            const void *pdata, int len);
    RmCANPacket               *RmGetCANPacket(const struct flexcan_packet *can_data);
    RmCANPacket               *RmGetCANPacket(const RmCANPacket *packet);
    RmCANPacket               *RmGetCANPacket(int priority, int src_id, int dest_id,
                                              const void *pdata, int len);
    RmCANPacket               *RmGetCANPacket(const Vector<struct flexcan_packet> packets);
    void                      RmRemovePacketId(KeyedVector<uint8_t, Vector<struct flexcan_packet> > &, ssize_t);
    void                      RmPushToRecvList(RmCANPacket *packet);
    void                      RmPushToSendList(RmCANPacket *packet);
    int                       RmHandleBroadcast(RmCANPacket *packet);

    Mutex                     mLock;       /* protection mClients   */
    Mutex                     mSendLock;   /* protection send list */
    Mutex                     mRecvLock;   /* protection recv list */
    Condition                 mRecvSignal;

    sp<Thread>                mTransThread;
    sp<Thread>                mRecvThread;

    const uint8_t             mTimeOutId;
    uint8_t                   mPacketsId[16];
    List<struct RmCANPacket*> mSendList;
    List<struct RmCANPacket*> mRecvList;
    /* <src_id, <packet_id, packets>> */
    KeyedVector<uint8_t, KeyedVector<uint8_t, Vector<struct flexcan_packet> > > mInCompletePackets;

    RmCANDevice               *mCANDevice;
    sp<Client>                mClients[RM_MODULE_NUM];
};

/**********************************************************************************/

}; /* namespace android */

#endif /* RM_CAN_SERVICE_H_ */
