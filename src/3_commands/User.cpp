#include "../../includes/User.hpp"

User::User(int fd) : _userSocketFd(fd), _isOperator(false), _isRegistered(false)
{
}

User::~User()
{
    close(_userSocketFd);
    for (std::vector<Channel*>::iterator it = _channels.begin();
         it != _channels.end(); ++it)
        (*it)->removeUser(*this);
    _channels.clear();
}

// GETTERS & SETTERS

int User::getFd() const
{
    return (_userSocketFd);
}

std::string User::getNick() const
{
    return (_nick);
}

std::string User::getUsername() const
{
    return (_username);
}

bool User::isOperator() const
{
    return (_isOperator);
}

bool User::isRegistered() const
{
    return (_isRegistered);
}

void User::setOperator(bool val)
{
    _isOperator = val;
}

void User::setRegistered(bool val)
{
    _isRegistered = val;
}

void User::setNick(std::string nick)
{
    _nick = nick;
}

void User::setUsername(std::string username)
{
    _username = username;
}
