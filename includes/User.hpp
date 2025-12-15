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

#include "Server.hpp"

class User
{
private:
    std::string _username;
    std::string _nick;
    int _userSocketFd;
    std::vector<Channel*> _channels;

    bool _isOperator;
    bool _isRegistered;

public:
    User(int fd);
    User(const User& other);
    User& operator=(const User& other);
    ~User();

	// CHANNELS RELATED
	void addChannel(Channel& channel);
    void removeChannel(Channel& channel);

	// GETTERS
    int getFd() const;
    std::string getNick() const;
    std::string getUsername() const;
    bool isOperator() const;
    bool isRegistered() const;
    
	// SETTERS
	void setOperator(bool val);
    void setRegistered(bool val);
    void setNick(std::string nick);
    void setUsername(std::string username);
};

//  We could make an operator class that inherits from User, but then to
//  distinguish a normal
//	user from an operator during run-time we should use some dynamic casting and
// it seems too complicated for nothing. 	Also inside of the child class
// operator we wouldn't really have any extra values to store or

#endif
