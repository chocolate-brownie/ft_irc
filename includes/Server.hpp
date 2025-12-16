#ifndef SERVER_HPP
#define SERVER_HPP

#define BACKLOG 10

#include <arpa/inet.h>
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep
#include <cstring>  // IWYU pragma: keep
#include <fcntl.h>  // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <netdb.h>
#include <netinet/in.h>
#include <sstream> // IWYU pragma: keep
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Server {
private:
  int _port;
  std::string _password;
  int _listener; // Listening socket descriptor

  const std::size_t _fdsize;        // max size of the room
  std::vector<struct pollfd> _pfds; // room for the connections
  struct pollfd makePollFds(int fd, short events) const;

  Server(const Server &other);
  Server &operator=(const Server &other);

  std::string portToString(int port);
  struct addrinfo *getAddressInfo();
  int getListenerSocket();

public:
  Server(int port, std::string password);
  ~Server();

  void start();
};

#endif
