#include "network_interface.h"
#include <string>

#include <cutils/sockets.h>


NetworkInterface::NetworkInterface() {
  mServerfd = 0;
  mClientfd = 0;
  mEventfd  = 0;
}


NetworkInterface* NetworkInterface::getInstance() {
  if (mInstance == nullptr) {
    mInstance = new NetworkInterface();
  }
  return mInstance;
}

int NetworkInterface::start() {
  mServerfd = socket_local_server(UNIX_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);

  if ( mServerfd <= 0 ) {
    LOGD("create server socket error");
    return SERVER_ERROR;
  } else {
    LOGD("create server socket: %d", mServerfd);
  }

  if (Util::set_socket_nonblocking(mServerfd) == -1) {
    LOGD("set nonblock error");
  } else {
    LOGD("set nonblock success");
  }

  event_loop();
  return 0;
}

int NetworkInterface::listen_server_event() {
  mEventfd = epoll_create1(0);
  if ( mEventfd < 0 ) {
    LOGD("init epoll server error");
    return SERVER_ERROR;
  } else {
    LOGD("init epoll server success");
  }
  struct epoll_event serverEvent;
  serverEvent.data.fd  = mServerfd;
  serverEvent.events   = EPOLLIN | EPOLLET;
  if (epoll_ctl(mEventfd, EPOLL_CTL_ADD, mServerfd, &serverEvent) < 0) {
    LOGD("epoll_ctl error");
    return SERVER_ERROR;
  } else {
    LOGD("add serverfd to epoll success");
  }

  return 0;
}

int NetworkInterface::event_loop() {
  
  listen_server_event();
  int eventSize;
  int i;
  while ( 1 ) {
    // LOGD("1. begin enter event_loop");

    eventSize = epoll_wait(mEventfd, mEvents, sizeof(mEvent), -1);
    LOGD("2. begin enter event_loop");

    if (eventSize == -1) {
      LOGD("epoll wait no event");
      // return SERVER_ERROR;
      continue;
    } else if (eventSize == 0) {
      LOGD("no socket read for read");
      continue;
    }

    // LOGD("get event_size is:%d", eventSize);
    for (i = 0; i < eventSize; i++) {
      if ((mEvents[i].events & EPOLLERR) || (mEvents[i].events & EPOLLHUP) || !(mEvents[i].events & EPOLLIN)) {
        _event_remove(mEvents[i].data.fd);
        LOGD("_event_remove");
        continue;
      }
      if (mEvents[i].data.fd == mServerfd) {
        accept_new_client();
      } else {
        LOGD("read data from client: %d ", mEvents[i].data.fd);
        handle_client_data(mEvents[i].data.fd);
      }
    }
  }

}


void NetworkInterface::accept_new_client() {
  size_t socklen = sizeof(struct sockaddr_in);
  mClientfd = accept(mServerfd, (struct sockaddr *)&mClientAddr, (socklen_t *)&socklen);
  if ( mClientfd < 0 ) {
    LOGD("accept socket error");
    return;
  }

  if ( (Util::set_socket_nonblocking(mClientfd)) < 0 ) {
    LOGD("set clientfd nonblocking error");
    return;
  }

  LOGD("get new connection: %d", mClientfd);
  epoll_register_event(mClientfd);

  event_add_new_client(mClientfd);
}

int NetworkInterface::_event_remove(int fd) {
  epoll_ctl(mEventfd, EPOLL_CTL_DEL, fd, NULL);
  close(fd);
  event_remove_client(fd);
  return 0;
}

void NetworkInterface::epoll_register_event(int fd) {
  struct epoll_event mEvent;
  // mEvent.events   = EPOLLIN | EPOLLET | EPOLLET;
  mEvent.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP;
  mEvent.data.fd = fd;
  if (epoll_ctl(mEventfd, EPOLL_CTL_ADD, fd, &mEvent) < 0) {
    LOGD("epoll add fd error");
    return;
  }
}

