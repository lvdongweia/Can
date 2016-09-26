LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

CAN_TOOLS := cansend candump

LOCAL_SRC_FILES := canutils.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE := canutils

LOCAL_SHARED_LIBRARIES := \
	libc \
	libutils

LOCAL_CFLAGS := -O2 -g -W -Wall
include $(BUILD_EXECUTABLE)

# Make #!/system/bin/canutils launchers for cansend/candump.
SYMLINKS := $(addprefix $(TARGET_OUT)/bin/,$(CAN_TOOLS))
$(SYMLINKS): CANUTILS_BINARY := $(LOCAL_MODULE)
$(SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Symlink: $@ -> $(CANUTILS_BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf $(CANUTILS_BINARY) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS)

