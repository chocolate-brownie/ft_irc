#include "../../includes/User.hpp"

#include "../../includes/Channel.hpp"

// FOR CONSTRUCTOR MAYBE WE HAVE TO PASS _hostname SINCE WE GET IT FROM ACCEPT()
// BUT IDK YET IF WE RUN IT BEFORE OR AFTER CREATING THE USER OBJ
User::User(int fd) : _userSocketFd(fd), _hostname("localhost"), _isRegistered(false) {}

User::~User() {
    close(_userSocketFd);

    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
        (*it)->removeUser(*this);
    _channels.clear();
}

// CHANNELS RELATED

void User::addChannel(Channel& channel) { _channels.push_back(&channel); }

void User::removeChannel(Channel& channel) {
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
        if (*it == &channel) {
            _channels.erase(it);
            return;
        }
    }
}

std::vector<Channel*> User::getChannels() { return (_channels); }

// GETTERS & SETTERS

int User::getFd() const { return (_userSocketFd); }

std::string User::getNick() const { return (_nick); }

std::string User::getUsername() const { return (_username); }

std::string User::getHostname() const { return (_hostname); }

std::string User::getRealname() const { return (_realname); }

std::string User::getPrefix() const { return (_nick + "!" + _username + "@" + _hostname); }

bool User::isRegistered() const { return (_isRegistered); }

void User::setRegistered(bool val) { _isRegistered = val; }

void User::setNick(const std::string& nick) { _nick = nick; }

void User::setUsername(const std::string& username) { _username = username; }

void User::setHostname(const std::string& host) { _hostname = host; }

void User::setRealname(const std::string& real) { _realname = real; }
