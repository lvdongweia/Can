LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    rm_can_server.cpp

LOCAL_SHARED_LIBRARIES := \
    librm_can_service \
    libutils \
    libcutils \
    libbinder

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../libcanservice

LOCAL_CFLAGS := -O2 -Wall -g
LOCAL_MODULE_TAGS := eng optional
LOCAL_MODULE:= canserver

include $(BUILD_EXECUTABLE)
