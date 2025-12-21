#ifndef USER_HPP
#define USER_HPP

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

class Server;
class Channel;

class User
{
private:
    int                   _userSocketFd;
    std::string           _nick;     // From NICK command
    std::string           _username; // From USER command (1st arg)
    std::string           _hostname; // IP from accept()
    std::string           _realname; // From USER command (Last arg)
    std::vector<Channel*> _channels;

    bool _isRegistered;

public:
    User(int fd);
    User(const User& other);
    User& operator=(const User& other);
    ~User();

    // CHANNELS RELATED
    void                  addChannel(Channel& channel);
    void                  removeChannel(Channel& channel);
    std::vector<Channel*> getChannels();

    // GETTERS
    int         getFd() const;
    std::string getNick() const;
    std::string getUsername() const;
    std::string getHostname() const;
    std::string getRealname() const;
    std::string getPrefix() const;
    bool        isRegistered() const;

    // SETTERS
    void setRegistered(bool val);
    void setNick(const std::string& nick);
    void setUsername(const std::string& username);
    void setHostname(const std::string& host);
    void setRealname(const std::string& real);
};

// We could make an operator class that inherits from User, but then to
// distinguish a normal
// user from an operator during run-time we should use some dynamic casting and
// it seems too complicated for nothing. Also inside of the child class
// operator we wouldn't really have any extra values to store so its more
// practical to just add a boolean isOperator instead

#endif
