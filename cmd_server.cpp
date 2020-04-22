#include "util.h"
#include "cmd_server.h"


CmdServer* CmdServer::mInstance = nullptr;

CmdServer* CmdServer::getInstance() {
  if (mInstance == nullptr) {
    mInstance = new CmdServer();
  }
  return mInstance;
}

int CmdServer::event_add_new_client(int fd) {
  return 0;
}

int CmdServer::event_remove_client(int fd) {
  return 0;
}

std::string CmdServer::exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    auto pipe = popen(cmd, "r"); // get rid of shared_ptr
    // if (!pipe) throw std::runtime_error("popen() failed!");
    if (!pipe) {
      result.append("_88_@err\n");
      return result;
    }
    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }
    auto rc = pclose(pipe);
    result.append("_88_");

    if (rc == -1) {
      result.append("@err\n");
    } else {
      result.append("@ok\n");
    }
    return result;
}

bool CmdServer::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void CmdServer::handle_client_data(int fd) {
  char buffer[2048];
  memset(buffer, 0, 2048);

  size_t bufflen = read(fd, buffer, 2048);

  if (bufflen == 0) {
    LOGD("socket closed or error");
    _event_remove(fd);
    return;
  }

  if (bufflen > 0) {
    std::string command(buffer);
    replace(command, "\n", " 2>&1\n");
    LOGD("execute command: %s", command.c_str());
    std::string result = exec(command.c_str());
    LOGD("result is: %s", result.c_str());
    send(fd, result.c_str(), result.length(), 0);
  }
}
