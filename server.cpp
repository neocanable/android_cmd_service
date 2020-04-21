#include <stdio.h>
#include <sys/un.h>

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
#include <errno.h>

#define SV_SOCK_PATH "\0sb.cmdservice.server"
#define BUF_SIZE 2048
#define BACKLOG 64

int main(int argc, char *argv[]) {    
    int fd_A[BACKLOG];              // accepted connection fd
    for (int j = 0; j < BACKLOG; j ++) {
        fd_A[j] = 0;
    }
    int sock_fd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_un server_addr; // server address information
    struct sockaddr_un client_addr; // connector's address information
    socklen_t sun_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SV_SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    // memcpy(server_addr.sun_path, SV_SOCK_PATH, sizeof(SV_SOCK_PATH) - 1);

    ret = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    if (ret == -1) {
        printf("bind socket error\n");
        exit(1);
    }
    if (listen(sock_fd, BACKLOG) == -1) {
        printf("listen socket error\n");
       exit(1);
    } else {
        printf("listen success\n");
    }
    fd_set fdsr;
    int maxsock;
    struct timeval tv;
    int conn_amount = 0; // current connection amount
    sun_size = sizeof(client_addr);
    maxsock = sock_fd;
    while (1) {
        // initialize file descriptor set
        FD_ZERO(&fdsr);
        FD_SET(sock_fd, &fdsr); // add fd
        // timeout setting
        tv.tv_sec = 60;
        tv.tv_usec = 0;
        // add active connection to fd set
        for (i = 0; i < BACKLOG; i++) {
            if (fd_A[i] != 0) {
                FD_SET(fd_A[i], &fdsr);
            }
        }
        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) { // error
           perror("select");
            break;
        } else if (ret == 0) { // time out
           printf("timeout\n");
            continue;
        }
        // check every fd in the set
        for (i = 0; i < conn_amount; i++) {
            if (FD_ISSET(fd_A[i], &fdsr)) // check which fd is ready
            {
                ret = recv(fd_A[i], buf, sizeof(buf), 0);
                if (ret <= 0) { // client close
//                    printf("ret : %d and client[%d] close\n", ret, i);
                    close(fd_A[i]);
                    FD_CLR(fd_A[i], &fdsr); // delete fd
                    fd_A[i] = 0;
                    conn_amount--;
                } else { // receive data
                    if (ret < BUF_SIZE)
                        memset(&buf[ret], '\0', 1); // add NULL('\0')
                        printf("client[%d] send : %s\n", i, buf);
                    for (int j = 0; j < sizeof(buf); j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    if (send(fd_A[i], buf, sizeof(buf), 0) < 0) {
//                        perror("send to client");
                    }
                }
            }
        }
        // check whether a new connection comes
        if (FD_ISSET(sock_fd, &fdsr)) // accept new connection
        {
            new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sun_size);
            if (new_fd <= 0) {
                printf("accept socket error\n");
                continue;
            }
            // add to fd queue
            if (conn_amount < BACKLOG) {
                fd_A[conn_amount++] = new_fd;
                if (new_fd > maxsock)
                    maxsock = new_fd;
            } else {
                send(new_fd, "bye", 4, 0);
                close(new_fd);
                break;
            }
        }
    }
    for (i = 0; i < BACKLOG; i++) {
        if (fd_A[i] != 0) {
            close(fd_A[i]);
            printf("Closed File Descriptor %d\n", i);
        }
    }
    close(sock_fd);
}  
