#include "../../includes/Server.hpp"
#include <arpa/inet.h>

void Server::handleNewConnection() {
  struct sockaddr_storage remoteaddr; // connector's address info
  socklen_t addrlen;
  int newfd; // Newly accepte()ed socket fd

  addrlen = sizeof(remoteaddr);
  if ((newfd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen)) ==
      -1)
    std::cerr << "accpet" << std::endl;
  else {
    // addToTheRoom(newfd); // TODO:
    std::cout << "✅ New connection from " << getClientIP(remoteaddr)
              << " on socket " << newfd << std::endl;
  }
}

void Server::handleClientData() {}

void Server::processConnections() {
  for (size_t i = 0; i < _pfds.size(); ++i) {
    // Check if someone's ready to read
    if (_pfds[i].revents & (POLLIN | POLLHUP)) {
      if (_pfds[i].fd == _listener) // Check if we are the listener
        handleNewConnection(); // If we are its a new connection. Handle it!!
      else
        handleClientData(); // Otherwise we're just a regular client
    }
  }
}
