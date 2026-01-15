#include "../../includes/Channel.hpp"

#include "../../includes/User.hpp"

#include <vector>

Channel::Channel(std::string name) : _name(name), _userlimit(0) {
    _key_mode    = false;
    _invite_mode = false;
    _topic_mode  = false;
    _limit_mode  = false;
}

Channel::~Channel() {}

// COMMUNICATION

// Sends a message to all users in the channel
int Channel::broadcast(std::string msg) {
    if (msg == "") return (-1);
    for (std::vector<User*>::iterator it = _users.begin(); it != _users.end(); it++) {
        send((*it)->getFd(), msg.c_str(), msg.length(), 0);
    }
    return (0);
}

// Sends a message to all users in the channel (but the sender)
int Channel::broadcast(User& source, std::string msg) {
    if (msg == "") return (-1);
    for (std::vector<User*>::iterator it = _users.begin(); it != _users.end(); it++) {
        if (*it == &source) continue;
        send((*it)->getFd(), msg.c_str(), msg.length(), 0);
    }
    return (0);
}

// USERS

User* Channel::isUserConnected(User& user) const {
    for (std::vector<User*>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
        if (*it == &user) return (*it);
    }
    return (NULL);
}

User* Channel::isUserConnected(const std::string& nick) const {
    for (std::vector<User*>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
        if ((*it)->getNick() == nick) return (*it);
    }
    return (NULL);
}

void Channel::addUser(User& user) { _users.push_back(&user); }

// void Channel::removeUser(User& user) {
//     for (std::vector<User*>::iterator it = _users.begin(); it != _users.end(); it++) {
//         if ((*it) == &user) {
//             _users.erase(it);
//             return;
//         }
//     }
// }

void Channel::removeUser(User& user) {
    for (std::vector<User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if ((*it)->getNick() == user.getNick()) {
            _users.erase(it);
            break;
        }
    }
    removeOperator(user);
}

std::string Channel::getUserList() const {
    std::string list = "";

    for (std::vector<User*>::const_iterator it = _users.begin(); it != _users.end(); it++) {
        if (this->isOperator(**it)) list += "@";
        list += (*it)->getNick() + " ";
    }
    return (list);
}

// OPERATORS

void Channel::addOperator(User& user) { _operators.push_back(&user); }

void Channel::removeOperator(User& user) {
    for (std::vector<User*>::iterator it = _operators.begin(); it != _operators.end(); it++) {
        if ((*it)->getNick() == user.getNick()) {
            _operators.erase(it);
            return;
        }
    }
}

bool Channel::isOperator(User& user) const {
    for (std::vector<User*>::const_iterator it = _operators.begin(); it != _operators.end(); it++) {
        if ((*it) == &user) return (true);
    }
    return (false);
}

// INVITES

void Channel::addInvited(User& user) { _invited.push_back(&user); }

void Channel::removeInvited(User& user) {
    for (std::vector<User*>::iterator it = _invited.begin(); it != _invited.end(); it++) {
        if ((*it) == &user) {
            _invited.erase(it);
            return;
        }
    }
}

bool Channel::isInvited(User& user) const {
    for (std::vector<User*>::const_iterator it = _invited.begin(); it != _invited.end(); it++) {
        if ((*it) == &user) return (true);
    }
    return (false);
}

// GETTERS & SETTERS
std::string Channel::getName() const { return (_name); }

void Channel::setName(std::string name) { _name = name; }

std::string Channel::getKey() const { return (_key); }

void Channel::setKey(std::string key) { _key = key; }

std::string Channel::getTopic() const { return (_topic); }

void Channel::setTopic(std::string topic) { _topic = topic; }

int Channel::getLimit() const { return (_userlimit); }

int Channel::getNumberUsers() const {
    int count = 0;
    for (std::vector<User*>::const_iterator it = _users.begin(); it != _users.end(); it++) count++;
    return (count);
}

std::string Channel::getMode() const {
    std::string msg = this->getName();
    if (!this->getInviteMode() && !this->getKeyMode() && !this->getLimitMode() &&
        !this->getTopicMode())
        return (msg);
    msg += " +";
    if (this->getInviteMode()) msg += "i";
    if (this->getKeyMode()) msg += "k";
    if (this->getTopicMode()) msg += "t";
    if (this->getLimitMode()) {
        std::stringstream ss;
        ss << this->getLimit();
        msg += ("l " + ss.str());
    }
    return (msg);
}

void Channel::setLimit(int limit) { _userlimit = limit; }

void Channel::setKeyMode(bool mode) { _key_mode = mode; }

void Channel::setLimitMode(bool mode) { _limit_mode = mode; }

void Channel::setTopicMode(bool mode) { _topic_mode = mode; }

void Channel::setInviteMode(bool mode) { _invite_mode = mode; }

bool Channel::getKeyMode() const { return (_key_mode); }

bool Channel::getInviteMode() const { return (_invite_mode); }

bool Channel::getTopicMode() const { return (_topic_mode); }

bool Channel::getLimitMode() const { return (_limit_mode); }
