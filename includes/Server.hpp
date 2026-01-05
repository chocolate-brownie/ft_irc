#ifndef SERVER_HPP
#define SERVER_HPP

#define BACKLOG 10

#include <arpa/inet.h>
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep
#include <cstring>  // IWYU pragma: keep
#include <fcntl.h>  // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream> // IWYU pragma: keep
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define RPL_WELCOME 001
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NICKNAMEINUSE 433
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANNELKEY 475
#define ERR_CHANOPRIVSNEEDED 482

#include "Channel.hpp"
#include "Parser.hpp"
#include "User.hpp"

class Server {
private:
    int         _port;
    std::string _password;
    int         _listener; // Listening socket descriptor

    const std::size_t          _fdsize; // max size of the room
    std::vector<struct pollfd> _pfds;   // room for the connections

    std::vector<Channel*> _channels;
    std::map<int, User*>  _users;

    Server(const Server& other);
    Server& operator=(const Server& other);

    // Type definition for a pointer to a member function of the Server class
    typedef void (Server::*CommandFunction)(User&, const ParsedCommand&);
    std::map<std::string, CommandFunction> _commandMap;

    void reply(int id, User& user, std::string arg1, std::string arg2);
    // Main methods regarding to the network-engine
    struct addrinfo* getAddressInfo();
    int              getListenerSocket();

    void processConnections();
    void handleNewConnection();
    bool handleClientData(int client_fd);
    void addToTheRoom(int fd, struct sockaddr_storage* remoteadd);
    void removeFromTheRoom(int fd);

    // Util methods regarding to the network-engine
    std::string   portToString(int port);
    struct pollfd makePollFds(int fd, short events) const;
    std::string   getClientIP(const struct sockaddr_storage& addr) const;

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

	void	addChannel(Channel* channel);
	void	rmvChannel(Channel* channel);
    Channel* getChannel(const std::string& name);
    User*    getUser(const std::string& name);
};

#endif
