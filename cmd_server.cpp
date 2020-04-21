#include "util.h"
#include "websocket_server.h"



CmdServer* CmdServer::mInstance = nullptr;

CmdServer* CmdServer::getInstance() {
  if (mInstance == nullptr) {
    mInstance = new CmdServer();
  }
  return mInstance;
}

int CmdServer::event_add_new_client(int fd) {
  clientManager->addWebsocketClient(mClientfd);
  return 0;
}

int CmdServer::event_remove_client(int fd) {
  clientManager->removeWebsocketClient(fd);
  clientManager->disconnectWebsocket(fd);
  return 0;
}

void CmdServer::handle_client_data(int fd) {
  // WebsocketClient *client = clientManager->getWebsocketClient(fd);
  // if (client == NULL) return;
  char buffer[2048];
  memset(buffer, 0, 2048);

  size_t bufflen = read(fd, buffer, 2048);

  if (bufflen == 0) {
    tcp_debug_print("socket closed or error");
    return;
  }

  if (bufflen > 0) {
    client->process(buffer, bufflen);
  }
}
