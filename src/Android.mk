LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mac.c \
	maclist.c \
	netinfo.c \
	main.c

LOCAL_CFLAGS := -DLISTDIR="\"/system/usr/share/macchanger\"" -DVERSION="\"1.7.0\""
LOCAL_MODULE := macchanger
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)
