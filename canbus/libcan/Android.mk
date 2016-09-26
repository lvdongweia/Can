LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    RobotModuleName.cpp \
    RmCANClient.cpp \
    IRmCAN.cpp \
    IRmCANService.cpp \
    IRmCANClient.cpp \
    IRmCANDeathNotifier.cpp \
    RmCANClient_c.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../include \

LOCAL_MODULE_TAGS := eng optional
LOCAL_CFLAGS := -O2 -Wall -g
LOCAL_MODULE:= librm_can

include $(BUILD_SHARED_LIBRARY)
