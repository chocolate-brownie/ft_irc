#include "../../includes/Server.hpp"

// Returns a Channel from the vector given its name
Channel* Server::getChannel(const std::string& name)
{
    for (std::vector<Channel*>::iterator it = _channels.begin();
         it != _channels.end(); it++)
    {
        if ((*it)->getName() == name)
            return (*it);
    }
    return (NULL);
}

User* Server::getUser(const std::string& name)
{
    for (std::vector<User*>::iterator it = _users.begin(); it != _users.end();
         it++)
    {
        if ((*it)->getNick() == name)
            return (*it);
    }
    return (NULL);
}

std::string intToString(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

void Server::reply(int id, User* user, std::string arg1, std::string arg2)
{
    // Build the Prefix: :ServerName <Code> <ClientNick>
    // :BBCServer 404 Mike : blablabla\r\n
    //  Note: If user is not registered (Nick is empty), standard uses "*"
    std::string user_nick =
        ((!user || user->getNick().empty()) ? "*" : user->getNick());
    std::string msg = ":BBCServer " + intToString(id) + " " + user_nick + " ";
    switch (id)
    {
        case RPL_WELCOME:
            msg += " :Welcome to the Internet Relay Network " +
                   user->getPrefix() + "\r\n";
            break;
        case RPL_TOPIC: // 332
            // Arg1 = channel name, Arg2 = topic
            msg += arg1 + " :" + arg2 + "\r\n";
            break;
        case RPL_NAMREPLY: // 353
            // Arg1 = channel name, Arg2 = list of names
            msg += "= " + arg1 + " :" + arg2 + "\r\n";
            break;
        case RPL_ENDOFNAMES: // 366
            msg += arg1 + " :End of /NAMES list\r\n";
            break;
        case ERR_NOSUCHNICK:
            msg += arg1 + " :No such nick/channel\r\n";
            break;
        case ERR_NOSUCHCHANNEL:
            msg += arg1 + " :No such channel\r\n";
            break;
        case ERR_NICKNAMEINUSE:
            msg += arg1 + " :Nickname is already in use\r\n";
            break;
        case ERR_USERNOTINCHANNEL:
            msg += arg1 + " :They aren't on that channel\r\n";
            break;
        case ERR_NOTONCHANNEL:
            msg += arg1 + " :You're not on that channel\r\n";
            break;
        case ERR_USERONCHANNEL:
            msg += arg1 + " :is already on channel\r\n";
            break;
        case ERR_NEEDMOREPARAMS:
            msg += arg1 + " :Not enough parameters\r\n";
            break;
        case ERR_CHANNELISFULL:
            msg += arg1 + " :Channel is full (+l)\r\n";
            break;
        case ERR_INVITEONLYCHAN:
            msg += arg1 + " :Invite needed to join (+i)\r\n";
            break;
        case ERR_BADCHANNELKEY:
            msg += arg1 + " :Cannot join channel (+k)\r\n";
            break;
        case ERR_CHANOPRIVSNEEDED:
            msg += arg1 + " :You're not channel operator\r\n";
            break;
        default:
            msg += " :Unknown error\r\n";
            break;
    }
    send(user->getFd(), msg.c_str(), msg.length(), 0);
}
void Server::cmdKick(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    User* target;
    std::string reply;

    // Checks if user is operator, if channel exists and if the target user is
    // in the channel
    if (!user.isOperator())
    {
        reply(ERR_CHANOPRIVSNEEDED, &user, channel->getName());
        return;
    }
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        reply(ERR_NOSUCHCHANNEL, &user, channel->getName());
        return;
    }
    if (!(target = channel->isUserConnected(cmd.args[1])))
    {
        reply(ERR_USERNOTINCHANNEL, &user,
                    target->getNick() + " " + channel->getName());
        return;
    }
    // When kicking user we remove him from the channel list and vice versa we
    // remove the channel from his list, and broadcast the message to inform the
    // channel
    std::string reply = user.getPrefix() + " KICK " + channel->getName() + " " +
                        target->getNick() + " : Kicked by operator\r\n";
    channel->broadcast(reply);
    channel->removeUser(*target);
    target->removeChannel(*channel);
}

void Server::cmdInvite(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    User* target;
    std::string reply;

    // Checks if channel exists and if the target user is
    // not already in the channel
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        this->reply(ERR_NOSUCHCHANNEL, &user, channel->getName(), NULL);
        return;
    }
    if ((target = channel->isUserConnected(cmd.args[1])))
    {
        reply(ERR_USERNOTINCHANNEL, target, NULL);
        return;
    }
    if (!(channel->isUserConnected(user.getNick())))
    {
        reply(ERR_USERNOTINCHANNEL, &user, NULL);
        return;
    }

    // IF INVITE MODE TRUE and USER IS NOT OPERATOR
    // we send the error else we invite the target
    if (!channel->getInviteMode() && !(user.isOperator()))
        reply(ERR_NOPRIVILEGES, &user, NULL);
    channel->addInvited(*target);
    reply =
        user.getNick() + " INVITED you to join #" + channel->getName() + "\r\n";
    send((*target).getFd(), reply.c_str(), reply.length(), 0);
    reply = target->getNick() + " has successfully been invite to #" +
            channel->getName() + "\r\n";
    send(user.getFd(), reply.c_str(), reply.length(), 0);
}

void Server::cmdTopic(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    std::string reply;
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        reply(ERR_NOSUCHCHANNEL, NULL, channel);
        return;
    }
    if (cmd.args.size() == 1)
    {
        reply = "#" + cmd.args[0] + " TOPIC: " + channel->getTopic() + "\r\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    } // IF IRINA CHECKS ALREADY I CAN TAKE OUT THE ELSE IF
    else if (cmd.args.size() == 2)
    {
        if (channel->getTopicMode() && !user.isOperator())
        {
            reply(ERR_NOPRIVILEGES, &user, NULL);
            return;
        }
        channel->setTopic(cmd.args[1]);
        reply = "#" + cmd.args[0] + " TOPIC updated: " + cmd.args[1] + "\r\n";
        channel->broadcast(reply);
        return;
    }
}

void Server::cmdMode(User& user, const ParsedCommand& cmd) {}

void Server::cmdJoin(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    std::string reply;

    // Check if channel exists
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        reply(ERR_NOSUCHCHANNEL, NULL, channel);
        return;
    }
    // When invite mode is active, if user is invited he can join else no
    if (channel->getInviteMode())
    {
        if (!channel->isInvited(user))
        {
            reply(ERR_INVITEONLYCHAN, NULL, channel);
            return;
        }
        channel->removeInvited(user);
    }
    // When key mode is active we check if the given password matches
    else if (channel->getKeyMode() &&
             cmd.args[1].compare(channel->getKey()) != 0)
    {
        reply(ERR_BADCHANNELKEY, NULL, channel);
        return;
    }
    // To join we add the user to the channel list and viceversa and we inform
    // the channel users
    reply = "#" + channel->getName() + " " + user.getNick() +
            "JOINED the channel\r\n";
    channel->addUser(user);
    user.addChannel(*channel);
    channel->broadcast(reply);
}

void Server::cmdPrivmsg(User& user, ParsedCommand& cmd)
{
    // Distinguish between #channel and user
    if (cmd.args[0][0] == '#')
    {
        cmd.args[0].erase(0, 1); // remove the #
        Channel* channel;
        if (!(channel = this->getChannel(cmd.args[0])))
        {
            reply(ERR_NOSUCHCHANNEL, NULL, channel);
            return;
        }
        if (!channel->isUserConnected(user))
        {
            reply(ERR_USERNOTINCHANNEL, &user, NULL);
            return;
        }
        channel->broadcast(user, cmd.args[1]);
        return;
    }
    User* target;
    if (!(target = this->getUser(cmd.args[1])))
    {
        reply(ERR_USERNOTCONNECTED, &user, NULL);
        return;
    }
    std::string reply = user.getNick() + " " + cmd.args[1] + "\r\n";
    send(target->getFd(), cmd.args[1].c_str(), cmd.args[1].size(), 0);
    send(user.getFd(), cmd.args[1].c_str(), cmd.args[1].size(), 0);
}

void Server::cmdNick(User& user, const ParsedCommand& cmd);
void Server::cmdUser(User& user, const ParsedCommand& cmd);
void Server::cmdPart(User& user, const ParsedCommand& cmd);
