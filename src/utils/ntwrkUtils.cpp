#include "../../includes/Server.hpp"

std::string Server::portToString(int port) {
  std::stringstream ss;
  ss << port;
  return ss.str();
}