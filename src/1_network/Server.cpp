#include "../../includes/Server.hpp"
#include <asm-generic/socket.h>
#include <sys/socket.h>

Server::Server(int port, std::string password)
    : _port(port), _password(password) {}

Server::~Server() {}

void Server::start() {
  struct addrinfo *servinfo = getAddressInfo();
  _serverSocketFd = createAndBindTheSocket(servinfo);

  if (listen(_serverSocketFd, BACKLOG) != 0) {
    std::cerr << "server: listen" << std::endl;
    exit(1);
  }

  struct sockaddr_storage their_addr; // connector's address info
  socklen_t addr_size;
  int new_fd;

  _pfds[0].fd = _serverSocketFd;
  _pfds[0].events =

      addr_size = sizeof(their_addr);
  new_fd = accept(_serverSocketFd, (struct sockaddr *)&their_addr, &addr_size);
  if (new_fd == -1)
    std::cerr << "Error: accept failed: " << std::strerror(errno) << std::endl;

  std::cout << "✅ A client successfully connected! (FD: " << new_fd << ")"
            << std::endl;
  close(new_fd);

  // sendAndReceiveData();
  // disconnectTheSocket();
}

struct addrinfo *Server::getAddressInfo() {
  struct addrinfo hints, *res;
  int status;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  std::string portStr = portToString(_port);

  if ((status = getaddrinfo(NULL, portStr.c_str(), &hints, &res)) == -1) {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    exit(1);
  }
  return res;
}

int Server::createAndBindTheSocket(struct addrinfo *servinfo) {
  struct addrinfo *p;
  int fd;
  int yes = 1;

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "server: socket" << std::endl;
      continue;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cerr << "setsockopt" << std::endl;
      exit(1);
    }

    if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
      std::cerr << "server: bind" << std::endl;
      close(fd);
      continue;
    }

    break; // Success!
  }

  freeaddrinfo(servinfo); // after binding the socket I am done with this struct

  if (p == NULL) {
    std::cerr << "server: failed to bind" << std::endl;
    exit(1);
  }

  return fd;
}