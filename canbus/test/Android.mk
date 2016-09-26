LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    test_main.cpp

LOCAL_SHARED_LIBRARIES := \
    librm_can \
    libutils \
    libcutils \
    libbinder

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../include

LOCAL_CFLAGS := -O2 -Wall -g
#LOCAL_MODULE:= test_can
#LOCAL_MODULE:= test_can_debug
LOCAL_MODULE_TAGS := eng optional
LOCAL_MODULE:= test_can_fault

include $(BUILD_EXECUTABLE)
