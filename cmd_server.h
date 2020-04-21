#ifndef __CMD_SERVER_H_
#define __CMD_SERVER_H_

#include "util.h"
#include "network_interface.h"

class CmdServer : public NetworkInterface
{
  public:
    static CmdServer* getInstance();
    int event_add_new_client(int fd);
    int event_remove_client(int fd);
    void handle_client_data(int fd);

    static CmdServer* mInstance;

  private:
    CmdServer():NetworkInterface(){};
    CmdServer(NetworkInterface const&);
    CmdServer& operator=(CmdServer const&);
    ~CmdServer();

};

#endif


