#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>  // IWYU pragma: keep
#include <string>
#include <vector>  // IWYU pragma: keep
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

class Server {
private:
    int         _port;
    std::string _password;
    int         _serverSocketFd;

    // to prevent accidental copying, bcs server class is non copyable
    Server(const Server& other);
    Server& operator=(const Server& other);

public:
    Server(int port, std::string password);
    ~Server();

    void start();
};

#endif
