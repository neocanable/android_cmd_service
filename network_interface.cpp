#include "network_interface.h"
#include <string>


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
  mServerfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  
  if ( mServerfd <= 0 ) {
    tcp_debug_print("create server socket error");
    return SERVER_ERROR;
  } else {
    tcp_debug_print("create server socket: %d", mServerfd);
  }

  memset(&mServerAddr, 0, sizeof(sockaddr_un));
  mServerAddr.sun_family = AF_LOCAL;

  // strncpy(mServerAddr.sun_path, UNIX_SOCKET_NAME, sizeof(mServerAddr.sun_path) - 1);
  // strcpy(mServerAddr.sun_path, "#sb.cmdservice.server");
  // mServerAddr.sun_path[0] = '\0';

  mServerAddr.sun_path[0] = '\0';
  strncpy(mServerAddr.sun_path+1, UNIX_SOCKET_NAME, strlen(UNIX_SOCKET_NAME));

  // unlink("\0sb.cmdservice.server");

  if (Util::set_socket_nonblocking(mServerfd) == -1) {
    tcp_debug_print("set nonblock error");
  } else {
    tcp_debug_print("set nonblock success");
  }

  if ( bind(mServerfd, (const struct sockaddr *) &mServerAddr, sizeof(struct sockaddr_un)) < 0 ) {
    // tcp_debug_print("bind socket path is: %s", socketStr.c_str());
    tcp_debug_print("bind socket success");
    return SERVER_ERROR;
  } else {
    tcp_debug_print("bind socket success");
  }

  if( listen(mServerfd, 5) < 0) {
    tcp_debug_print("listen socket error");
    return SERVER_ERROR;
  } else {
    tcp_debug_print("listen socket success");
  }

  event_loop();
  return 0;
}

int NetworkInterface::listen_server_event() {
  mEventfd = epoll_create(8);
  if ( mEventfd < 0 ) {
    tcp_debug_print("init epoll server error");
    return SERVER_ERROR;
  } else {
    tcp_debug_print("init epoll server success");
  }
  struct epoll_event serverEvent;
  serverEvent.data.fd  = mServerfd;
  serverEvent.events   = EPOLLIN | EPOLLET;
  if (epoll_ctl(mEventfd, EPOLL_CTL_ADD, mServerfd, &serverEvent) < 0) {
    tcp_debug_print("epoll_ctl error");
    return SERVER_ERROR;
  } else {
    tcp_debug_print("add serverfd to epoll success");
  }

  return 0;
}

int NetworkInterface::event_loop() {
  
  listen_server_event();
  int eventSize;
  int i;
  while ( 1 ) {
    // tcp_debug_print("1. begin enter event_loop");

    eventSize = epoll_wait(mEventfd, mEvents, sizeof(mEvent), -1);
    tcp_debug_print("2. begin enter event_loop");

    if (eventSize == -1) {
      tcp_debug_print("epoll wait no event");
      // return SERVER_ERROR;
      continue;
    } else if (eventSize == 0) {
      tcp_debug_print("no socket read for read");
      continue;
    }

    // tcp_debug_print("get event_size is:%d", eventSize);
    for (i = 0; i < eventSize; i++) {
      if ((mEvents[i].events & EPOLLERR) || (mEvents[i].events & EPOLLHUP) || !(mEvents[i].events & EPOLLIN)) {
        event_remove_client(mEvents[i].data.fd);
      }
      if (mEvents[i].data.fd == mServerfd) {
        accept_new_client();
      } else {
        tcp_debug_print("read data from client: %d ", mEvents[i].data.fd);
        handle_client_data(mEvents[i].data.fd);
      }
    }
  }

}


void NetworkInterface::accept_new_client() {
  size_t socklen = sizeof(struct sockaddr_in);
  mClientfd = accept(mServerfd, (struct sockaddr *)&mClientAddr, (socklen_t *)&socklen);
  if ( mClientfd < 0 ) {
    tcp_debug_print("accept socket error");
    return;
  }

  if ( (Util::set_socket_nonblocking(mClientfd)) < 0 ) {
    tcp_debug_print("set clientfd nonblocking error");
    return;
  }

  tcp_debug_print("get new connection: %d", mClientfd);
  epoll_register_event(mClientfd);

  event_add_new_client(mClientfd);
}

int NetworkInterface::_event_remove(int fd) {
  return 0;
}

void NetworkInterface::epoll_register_event(int fd) {
  struct epoll_event mEvent;
  mEvent.events   = EPOLLIN | EPOLLET | EPOLLET;
  mEvent.data.fd = fd;
  if (epoll_ctl(mEventfd, EPOLL_CTL_ADD, fd, &mEvent) < 0) {
    tcp_debug_print("epoll add fd error");
    return;
  }
}

