#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

class Server {
private:
    int         _port;
    std::string _password;
    int         _serverSocketFd;

public:
    Server(int port, std::string password);
    ~Server();

    void start();
};

#endif
