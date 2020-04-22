#ifndef __CMD_SERVER_H_
#define __CMD_SERVER_H_

#include "util.h"
#include "network_interface.h"
#include <array>
#include <string>
#include <iostream>



class CmdServer : public NetworkInterface
{
  public:
    static CmdServer* getInstance();
    int event_add_new_client(int fd);
    int event_remove_client(int fd);
    void handle_client_data(int fd);
    std::string exec(const char* cmd);
    bool replace(std::string& str, const std::string& from, const std::string& to);


    static CmdServer* mInstance;

  private:
    CmdServer():NetworkInterface(){};
    CmdServer(NetworkInterface const&);
    CmdServer& operator=(CmdServer const&);
    ~CmdServer() {};

};

#endif


