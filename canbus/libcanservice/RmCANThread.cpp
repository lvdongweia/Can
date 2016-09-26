/*
 */

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "RmCANService.h"
#include "RmCANThread.h"
#include "rm_can_log.h"

#undef LOG_TAG
#define LOG_TAG "RM_CAN_THREAD"

namespace android
{

/******************* can transport thread *******************/

RmCANTransThread::RmCANTransThread(const wp<RmCANService> &service, int sock)
{
    mService = service;
    mSockFd = sock;
}

RmCANTransThread::~RmCANTransThread()
{
}

bool RmCANTransThread::threadLoop()
{
#if RUNNING_IN_SIMULATOR
    return false;
#endif
    sp<RmCANService> service = mService.promote();
    if (service == NULL) return false;

    if (mSockFd == -1)
    {
        LOGE(LOG_TAG, "socket invaild, re-init");
        sleep(1);
        int ret = service->RmReInitCANDevice(&mSockFd);

        if (ret != 0)
            LOGE(LOG_TAG, "re-init CAN device error: %s", strerror(errno));
        return true;
    }

    fd_set rfds, wfds;
    struct timeval to = {0, 10 * 1000};

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    FD_SET(mSockFd, &rfds);
    if (!service->isSendListEmpty()) FD_SET(mSockFd, &wfds);

    int ret = select(mSockFd + 1, &rfds, &wfds, NULL, &to);
    if (ret < 0)
    {
        LOGE(LOG_TAG, "sock select error: %s", strerror(errno));
        sleep(1);
        service->RmReInitCANDevice(&mSockFd);
    }
    else if (ret > 0)
    {
        if (FD_ISSET(mSockFd, &rfds))
        {
            service->RmRecvFromCAN();
        }

        if (FD_ISSET(mSockFd, &wfds))
        {
            service->RmSendToCAN();
        }
    }

    return true;
}

/***************** can recv thread ****************/

RmCANReceiverThread::RmCANReceiverThread(const wp<RmCANService> &service)
{
    mService = service;
}

RmCANReceiverThread::~RmCANReceiverThread()
{
}

bool RmCANReceiverThread::threadLoop()
{
    sp<RmCANService> service = mService.promote();
    if (service == NULL) return false;

    service->RmRecvFromList();

    //usleep(1 * 1000); /* 1ms */

    return true;
}

}; /* namespace android */
