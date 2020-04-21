LOCAL_PATH      := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_MODULE    := target
LOCAL_SRC_FILES := cmd_server.cpp network_interface.cpp main.cpp
# LOCAL_SRC_FILES := server.cpp
LOCAL_LDLIBS    := -llog




# APP_ALLOW_MISSING_DEPS := true
# LOCAL_SHARED_LIBRARIES := PlatformProperties

include $(BUILD_EXECUTABLE)
