/*
 */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netutils/ifc.h>
#include <netutils/dhcp.h>

#include "RmCANDevice.h"
#include "rm_can_log.h"

#define LOG_TAG "RM_CAN_DEVICE"

namespace android
{

RmCANDevice::RmCANDevice()
{
    mCANDevice = NULL;
    mCANModule = NULL;
    mSockFd    = -1;
    mIsOpenedFlag = false;

    RmInitCANDevice();
}

RmCANDevice::~RmCANDevice()
{
    RmCloseCANDevice();
}

int RmCANDevice::RmGetCANModule()
{
    if (mCANModule != NULL) return 0;

    int ret = -1;
    ret = hw_get_module(FLEXCAN_HARDWARE_MODULE_ID,
            (const struct hw_module_t**)&mCANModule);

    if (ret != 0)
    {
        LOGE(LOG_TAG, "Couldn't load %s module (%s)",
                FLEXCAN_HARDWARE_MODULE_ID, strerror(errno));
        return -1;
    }
    LOGI(LOG_TAG, "load %s module successfully !!!", FLEXCAN_HARDWARE_MODULE_ID);
    return 0;
}
int RmCANDevice::RmInitCANDevice()
{
    /* device is not exist or not UP status */
    if (!isDeviceAvailable())
    {
        LOGE(LOG_TAG, "CAN device is not exist or DOWN state");
        return -1;
    }

    /* load module */
    if (RmGetCANModule() != 0) return -1;

    if (mCANDevice != NULL)
        RmCloseCANDevice();

    /* open device */
    int ret = -1;
    struct hw_module_t *module = &(mCANModule->common);
    ret = module->methods->open(module,
            FLEXCAN_HARDWARE_DEVICE_ID,
            (struct hw_device_t**)&mCANDevice);

    if (ret != 0)
    {
        LOGE(LOG_TAG, "open can device failed");
        return -1;
    }
    LOGI(LOG_TAG, "init %s module finished", FLEXCAN_HARDWARE_MODULE_ID);

    mIsOpenedFlag = true;
    mSockFd = mCANDevice->sockfd;

    return 0;
}

bool RmCANDevice::isDeviceAvailable()
{
    DIR *d;
    struct dirent *de;
    bool ret = false;

    if (ifc_init())
    {
        LOGE(LOG_TAG, "ifc init failed: %s", strerror(errno));
        return ret;
    }

    d = opendir("/sys/class/net");
    if(d == 0) return false;

    while ((de = readdir(d)))
    {
        if (de->d_name[0] == '.') continue;
        //ROBOT_CAN_INTERFACE by defined in rm_flexcan.h
        if (strcmp(de->d_name, ROBOT_CAN_INTERFACE) == 0)
        {
            unsigned addr, flags;
            int prefixLength;

            if (ifc_get_info(de->d_name, &addr, &prefixLength, &flags) == 0)
            {
                ret = flags & 1 ? true : false;
            }
            break;
        }
    }
    closedir(d);

    ifc_close();
    return ret;
}

int RmCANDevice::RmCloseCANDevice()
{
    if (!mIsOpenedFlag) return -1;

    int ret = mCANDevice->common.close((struct hw_device_t*)mCANDevice);
    return ret;
}

int RmCANDevice::RmGetDeviceFd()
{
    return mSockFd;
}

bool RmCANDevice::isOpened()
{
    return mIsOpenedFlag;
}

int RmCANDevice::RmCANSend(const struct flexcan_packet *packet)
{
    if (!mIsOpenedFlag) return -1;

    int ret = mCANDevice->flexcan_send(mCANDevice, packet);
    while (ret < 0 && errno == ENOBUFS)
    {
        usleep(3 * 1000);   //sleep 3ms, again send
        ret = mCANDevice->flexcan_send(mCANDevice, packet);
    }

    return ret;
}

int RmCANDevice::RmCANRecv(struct flexcan_packet *packet)
{
    if (!mIsOpenedFlag || !packet) return -1;

    /* set 0 in external */
    //memset(packet, 0, sizeof(struct flexcan_packet));

    int ret = mCANDevice->flexcan_recv(mCANDevice, packet);
    if (ret != 0)
    {
        LOGE(LOG_TAG, "recv flexcan packet error");
        return -1;
    }

    return 0;
}

}; /* namespace android */
