#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <android/log.h>



#define _DEBUG 0

#define SERVER_ERROR -1

#define MAX_CLIENT_SIZE 1024

#define UNIX_SOCKET_NAME "my.cmd.service"

#define  LOG_TAG "CmdService"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


typedef int SOCKET;

class Util {
  public:
    static int set_socket_nonblocking(int socketfd) {
        if (fcntl(socketfd, F_SETFL, fcntl(socketfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
            return -1;
        }
        return 0;
    }
};

#endif


