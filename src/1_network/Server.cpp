#include "../../includes/Server.hpp"
#include <asm-generic/socket.h>
#include <sys/socket.h>

Server::Server(int port, std::string password)
    : _port(port), _password(password) {}

Server::~Server() {}

void Server::start() {
  struct addrinfo *servinfo = getAddressInfo();
  _serverSocketFd = createAndBindTheSocket(servinfo);

  /* backlog is the number of connections allowed on the incoming queue. What
   * does that mean? Well, incoming connections are going to wait in this queue
   * until you accept() them (see below) and this is the limit on how many can
   * queue up. Most systems silently limit this number to about 20; you can
   * probably get away with setting it to 5 or 10.*/
  if (listen(_serverSocketFd, BACKLOG) != 0) {
    std::cerr << "Error: Listening failed" << std::endl;
    freeaddrinfo(servinfo);
    exit(1);
  }

  struct sockaddr_storage their_addr; // connector's address info
  socklen_t addr_size;
  int new_fd;

  /* For the moment I will accept the connection thinking that more than one
   * client would not try to connect to the server*/
  addr_size = sizeof(their_addr);
  new_fd = accept(_serverSocketFd, (struct sockaddr *)&their_addr, &addr_size);
  if (new_fd == -1)
    std::cerr << "Error: accept failed: " << std::strerror(errno) << std::endl;

  std::cout << "✅ A client successfully connected! (FD: " << new_fd << ")"
            << std::endl;
  close(new_fd);

  // sendAndReceiveData();
  // disconnectTheSocket();

  freeaddrinfo(servinfo);
}

/* Check available address results (IPv4 or IPv6) and attempt to create
 * and bind a socket to the first valid one. Loop through the results to handle
 * both IPv4 and IPv6. While mandatory is IPv4-only, this approach provides
 * protocol-agnostic robustness correctly */
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

  freeaddrinfo(servinfo);

  if (p == NULL) {
    std::cerr << "server: failed to bind" << std::endl;
    exit(1);
  }

  return fd;
}