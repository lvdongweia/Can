### flexcan.default.so
### flex HAL interface

LOCAL_PATH := $(call my-dir)

PRIVATE_LOCAL_CFLAGS := -O2 -g -W -Wall \
	-DPF_CAN=29 \
	-DAF_CAN=PF_CAN

# HAL module implemenation stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_SRC_FILES := rm_flexcan.c

LOCAL_CFLAGS := $(PRIVATE_LOCAL_CFLAGS)
LOCAL_MODULE_TAGS := optional eng
LOCAL_MODULE := flexcan.default
include $(BUILD_SHARED_LIBRARY)
