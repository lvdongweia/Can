LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=  \
    RmCANService.cpp \
    RmCANThread.cpp \
    RmCANDevice.cpp

LOCAL_SHARED_LIBRARIES := \
    libbinder \
    libcutils \
    libutils \
    libnetutils \
    libhardware \
    librm_can

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../include  \
    $(LOCAL_PATH)/

LOCAL_CFLAGS := -O2 -Wall -g

ifneq ($(BUILD_EMULATOR),true)
LOCAL_CFLAGS += -DRUNNING_IN_SIMULATOR=1
else
LOCAL_CFLAGS += -DRUNNING_IN_SIMULATOR=0
endif

ifeq ($(TARGET_BOARD_PLATFORM),)
LOCAL_CFLAGS += -DRUNNING_IN_SIMULATOR=1
endif

LOCAL_MODULE_TAGS := eng optional

LOCAL_MODULE:= librm_can_service

include $(BUILD_SHARED_LIBRARY)
