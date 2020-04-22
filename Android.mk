LOCAL_PATH      := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_MODULE    := cmdserver
LOCAL_SRC_FILES := cmd_server.cpp network_interface.cpp main.cpp
LOCAL_LDLIBS    := -llog


APP_ALLOW_MISSING_DEPS := true
LOCAL_SHARED_LIBRARIES:= libcutils

include $(BUILD_EXECUTABLE)
