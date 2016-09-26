/*
 */

#ifndef RM_CAN_DEVICE_H_
#define RM_CAN_DEVICE_H_

#include <hardware/hardware.h>
#include <hardware/rm_flexcan.h>

/* call HAL interface */

namespace android
{

class RmCANDevice
{
public:
    RmCANDevice();
    ~RmCANDevice();

    int RmGetDeviceFd();
    bool isOpened();

    int RmInitCANDevice();

    int RmCANSend(const struct flexcan_packet *packet);

    int RmCANRecv(struct flexcan_packet *packet);


private:
    int RmCloseCANDevice();
    bool isDeviceAvailable();
    int RmGetCANModule();

    bool mIsOpenedFlag;
    int mSockFd;

    struct flexcan_device_t *mCANDevice;
    struct flexcan_module_t *mCANModule;
};

}; /* namespace android */

#endif /* RM_CAN_DEVICE_H_ */
