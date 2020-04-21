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



#define _DEBUG 1

#define SERVER_ERROR -1

#define MAX_CLIENT_SIZE 1024

#define UNIX_SOCKET_NAME "sb.cmdservice.server"

#define tcp_debug_print(FMT, ARGS...) do { \
    if ( _DEBUG ) \
        fprintf(stdout, "[%s %s] %s %s:%d " FMT "\n", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__, ## ARGS); \
    } while (0)


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


