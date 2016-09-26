/*
*/

#define LOG_TAG "RM_CAN_SERVER"
#define LOG_NDEBUG 1

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>

#include "RmCANService.h"
#include "rm_can_log.h"

using namespace android;

int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);
    sp<ProcessState> proc(ProcessState::self());
    sp<IServiceManager> sm = defaultServiceManager();
    LOGI(LOG_TAG, "ServiceManager: %p", sm.get());
    RmCANService::instantiate();
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
}
