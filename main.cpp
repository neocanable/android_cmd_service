#include <thread>

#include "cmd_server.h"

void startCmdServer() {
  tcp_debug_print("start tcp server");
  CmdServer *server = nullptr;
  server = CmdServer::getInstance();
  server->start();
}



int main() {

  startCmdServer();
  return 0;
}
