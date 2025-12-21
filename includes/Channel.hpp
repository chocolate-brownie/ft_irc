#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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

class User;

class Channel
{
private:
    std::string        _name;
    std::string        _topic;
    std::string        _key;
    std::vector<User*> _users;
    std::vector<User*> _operators;
    std::vector<User*> _invited;
    int                _userlimit;

    bool _key_mode;
    bool _invite_mode;
    bool _topic_mode;
    bool _limit_mode;

    Channel(const Channel& other);
    Channel& operator=(const Channel& other);

public:
    Channel(std::string name);
    ~Channel();

    // COMMUNICATION
    int broadcast(std::string msg);
    int broadcast(User& source, std::string msg);

    // USERS
    User*       isUserConnected(User& user) const;
    User*       isUserConnected(const std::string& nick) const;
    void        addUser(User& user);
    void        removeUser(User& user);
    std::string getUserList() const;

    // OPERATORS
    void addOperator(User& client);
    void removeOperator(User& client);
    bool isOperator(User& client) const;

    // INVITES
    void addInvited(User& client);
    void removeInvited(User& client);
    bool isInvited(User& client) const;

    // GETTERS
    std::string getName() const;
    std::string getKey() const;
    std::string getTopic() const;
    int         getLimit() const;
    bool        getKeyMode() const;
    bool        getInviteMode() const;
    bool        getTopicMode() const;
    bool        getLimitMode() const;

    // SETTERS
    void setName(std::string name);
    void setKey(std::string key);
    void setTopic(std::string topic);
    void setLimit(int limit);
    void setKeyMode(bool mode);
    void setInviteMode(bool mode);
    void setTopicMode(bool mode);
    void setLimitMode(bool mode);
};

#endif
