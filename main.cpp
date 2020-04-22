#include <sys/stat.h>

#include "cmd_server.h"

void startCmdServer() {
  LOGD("start tcp server");
  CmdServer *server = nullptr;
  server = CmdServer::getInstance();
  server->start();
}



int main() {
  pid_t pid, sid;
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  umask(0);
  sid = setsid();
  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  if ((chdir("/data/local/tmp")) < 0) {
    /* Log the failure */
    exit(EXIT_FAILURE);
  }
  setbuf(stdout, NULL);

  startCmdServer();
  return 0;
}
