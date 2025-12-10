#include "../../includes/Server.hpp"

Server::Server(int port, std::string password)
    : _port(port), _password(password) {}

Server::~Server() {}

void Server::start() {
  struct addrinfo *servinfo = getAddressInfo();
  int sockfd = createAndBindTheSocket(servinfo);

  /* backlog is the number of connections allowed on the incoming queue. What
   * does that mean? Well, incoming connections are going to wait in this queue
   * until you accept() them (see below) and this is the limit on how many can
   * queue up. Most systems silently limit this number to about 20; you can
   * probably get away with setting it to 5 or 10.*/
  if (listen(sockfd, 5) != 0) {
    std::cerr << "Error: Listening failed" << std::endl;
    freeaddrinfo(servinfo);
    exit(1);
  }

  // acceptAConnection();
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

  if ((status = getaddrinfo(NULL, portStr.c_str(), &hints, &res)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    exit(1);
  }
  return res;
}

int Server::createAndBindTheSocket(struct addrinfo *servinfo) {
  struct addrinfo *p;
  int socket_fd;
  for (p = servinfo; p != NULL; p = p->ai_next) {
    socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (socket_fd == -1) {
      std::cerr << "Error: Socket creation failed, trying next..." << std::endl;
      continue;
    }

    if (bind(socket_fd, p->ai_addr, p->ai_addrlen) != 0) {
      std::cerr << "Error: Binding failed for the current socket" << std::endl;
      close(socket_fd);
      continue;
    }

    break; // Success!
  }

  if (p == NULL) {
    std::cerr << "Error: Failed to bind anything" << std::endl;
    exit(1);
  }

  return socket_fd;
}