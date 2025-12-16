#include "../../includes/Server.hpp"

std::string Server::portToString(int port) {
  std::stringstream ss;
  ss << port;
  return ss.str();
}

struct pollfd Server::makePollFds(int fd, short events) const {
  struct pollfd p;
  p.fd = fd;
  p.events = events;
  p.revents = 0;
  return p;
}
