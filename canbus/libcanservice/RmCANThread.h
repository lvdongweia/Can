/*
 */

#ifndef RM_CAN_THREAD_H_
#define RM_CAN_THREAD_H_

#include <utils/threads.h>

namespace android
{

class RmCANService;

/************************** can receiver thread ********************/

struct RmCANReceiverThread : public Thread
{
    RmCANReceiverThread(const wp<RmCANService> &service);

protected:
    virtual ~RmCANReceiverThread();

    virtual bool threadLoop();

private:
    wp<RmCANService> mService;

    RmCANReceiverThread(const RmCANReceiverThread &);
    RmCANReceiverThread &operator=(const RmCANReceiverThread &);
};

/*************************** can transport thread **********************/

struct RmCANTransThread : public Thread
{
    RmCANTransThread(const wp<RmCANService> &service, int sock);

protected:
    virtual ~RmCANTransThread();

    virtual bool threadLoop();

private:
    wp<RmCANService> mService;
    int mSockFd;

    RmCANTransThread(const RmCANTransThread &);
    RmCANTransThread &operator=(const RmCANTransThread &);
};

}; /* namespace android */

#endif /* RM_CAN_THREAD_H_ */
