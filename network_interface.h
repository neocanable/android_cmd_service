#ifndef __NETWORK_INTERFACE_H_

#define __NETWORK_INTERFACE_H_

#include "util.h"

#include <sys/epoll.h>

class NetworkInterface {
  public:
    static NetworkInterface *getInstance();


    void accept_new_client();
    int start();
    int event_loop();
    int _event_remove(int fd);

    virtual int event_add_new_client(int fd){return 0;};
    virtual int event_remove_client(int fd){return 0;};
    virtual void handle_client_data(int fd){};
  public:
    NetworkInterface();
    NetworkInterface(NetworkInterface const&);
    NetworkInterface& operator=(NetworkInterface const&);
    ~NetworkInterface();

  private:
    int listen_server_event();
  protected:
    static NetworkInterface* mInstance;

    int mServerfd;
    int mClientfd;
    int mEventfd; // epoll server fd

    struct sockaddr_in mServerAddr;
    struct sockaddr_in mClientAddr;

    struct epoll_event mEvent;
    struct epoll_event mEvents[MAX_CLIENT_SIZE]; // epoll events

  private:
    void epoll_register_event(int fd);

};
#endif
