#ifndef SERVER_HPP
#define SERVER_HPP

#define BACKLOG 10

#include <arpa/inet.h>
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep
#include <cstring>  // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream> // IWYU pragma: keep
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector> // IWYU pragma: keep

class Server {
private:
  int _port;
  std::string _password;
  int _serverSocketFd;

  Server(const Server &other);
  Server &operator=(const Server &other);

  std::string portToString(int port);
  struct addrinfo *getAddressInfo();
  int createAndBindTheSocket(struct addrinfo *servinfo);

public:
  Server(int port, std::string password);
  ~Server();

  void start();
};

#endif
