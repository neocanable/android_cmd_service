ndk-build  NDK_PROJECT_PATH=. APP_BUILD_PATH=./Android.mk NDK_APPLICATION_MK=./Application.mk APP_BUILD_SCRIPT=Android.mk

adb push obj/local/arm64-v8a/target /data/local/tmp/
adb shell chmod 777 /data/local/tmp/target

# adb shell /data/local/tmp/target
