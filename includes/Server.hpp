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

#include <map>

#include "Channel.hpp"
#include "Parser.hpp"
#include "User.hpp"

class Server
{
private:
    int _port;
    std::string _password;
    int _serverSocketFd;

    std::vector<Channel*> _channels;
    std::vector<User*> _users;

    Server(const Server& other);
    Server& operator=(const Server& other);

    std::string portToString(int port);
    struct addrinfo* getAddressInfo();
    int createAndBindTheSocket(struct addrinfo* servinfo);

    // Type definition for a pointer to a member function of the Server class
    typedef void (Server::*CommandFunction)(User&, const ParsedCommand&);
    std::map<std::string, CommandFunction> _commandMap;

public:
    Server(int port, std::string password);
    ~Server();

    void start();

    void executeCommand(User& user, const ParsedCommand& cmd);

    void cmdKick(User& user, const ParsedCommand& cmd);
    void cmdInvite(User& user, const ParsedCommand& cmd);
    void cmdTopic(User& user, const ParsedCommand& cmd);
    void cmdMode(User& user, const ParsedCommand& cmd);
    void cmdJoin(User& user, const ParsedCommand& cmd);
    void cmdPrivmsg(User& user, const ParsedCommand& cmd);
    void cmdNick(User& user, const ParsedCommand& cmd);
    void cmdUser(User& user, const ParsedCommand& cmd);
    void cmdPart(User& user, const ParsedCommand& cmd);

    Channel* getChannel(const std::string& name);
};

#endif
