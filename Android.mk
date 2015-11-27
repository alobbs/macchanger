LOCAL_PATH := $(call my-dir)

######################################################
###             macchanger                         ###
######################################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
    src/mac.c \
    src/maclist.c \
    src/netinfo.c \
    src/main.c \

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE := macchanger
LOCAL_CFLAGS += -DLISTDIR="\"/system/etc/macchanger\"" -DVERSION="\"Android\""
include $(BUILD_EXECUTABLE)

######################################################
###                 OUI.list                       ###
######################################################
include $(CLEAR_VARS)
LOCAL_MODULE := OUI.list
LOCAL_SRC_FILES := data/OUI.list
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := debug
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/macchanger
include $(BUILD_PREBUILT)

######################################################
###                 wireless.list                  ###
######################################################
include $(CLEAR_VARS)
LOCAL_MODULE := wireless.list
LOCAL_SRC_FILES := data/wireless.list
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := debug
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/macchanger
include $(BUILD_PREBUILT)
