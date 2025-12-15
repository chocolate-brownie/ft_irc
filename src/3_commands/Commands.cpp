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

void Server::cmdKick(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    User* target;
    std::string reply;

    // Checks argument size   NB MAYBE IRINA ALREADY CHECKS THIS
    if (cmd.args.size() < 2)
    {
        reply = user.getNick() + " KICK :Not enough parameters\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    // Checks if user is operator, if channel exists and if the target user is
    // in the channel
    if (!user.isOperator())
    {
        reply = user.getNick() + " :Operator permission required\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        reply = cmd.args[0] + " :No such channel\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    if (!(target = channel->isUserConnected(cmd.args[1])))
    {
        reply = cmd.args[1] + " :The user is not connected to the channel\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }

    reply = user.getNick() + " KICK #" + channel->getName() + " " +
            target->getNick() + "\n";
    channel->broadcast(reply);
    channel->removeUser(*target);
}

void Server::cmdInvite(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    User* target;
    std::string reply;

    // MAYBE IRINA CHECK THIS ALREADY
    if (cmd.args.size() < 2)
    {
        reply = user.getNick() + " INVITE :Not enough parameters\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    // Checks if channel exists and if the target user is
    // not already in the channel
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        reply = "#" + cmd.args[0] + " : Channel not found\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    if ((target = channel->isUserConnected(cmd.args[1])))
    {
        reply =
            cmd.args[1] + " :The user is already connected to the channel\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    if (!(channel->isUserConnected(user.getNick())))
    {
        reply = user.getNick() + " :You're not connected to the channel\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }

    // IF INVITE MODE TRUE
    // We add the target in the invited list of the channel, we send a message
    // to inform him and a confirm reply to the sender
    if (!channel->getInviteMode())
    {
        channel->addInvited(*target);
        reply = user.getNick() + " INVITED you to join #" + channel->getName() +
                "\n";
        send((*target).getFd(), reply.c_str(), reply.length(), 0);
        reply = target->getNick() + " has successfully been invite to #" +
                channel->getName() + "\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
    }
    else
    {
        if (!(user.isOperator()))
        {
            reply = user.getNick() + " :Operator permissions needed\n";
            send(user.getFd(), reply.c_str(), reply.length(), 0);
            return;
        }
    }
}

void Server::cmdTopic(User& user, const ParsedCommand& cmd)
{
    Channel* channel;
    std::string reply;
    if (!(channel = this->getChannel(cmd.args[0])))
    {
        reply = "#" + cmd.args[0] + " :Channel not found\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    }
    if (cmd.args.size() == 1)
    {
        reply = "#" + cmd.args[0] + " TOPIC: " + channel->getTopic() + "\n";
        send(user.getFd(), reply.c_str(), reply.length(), 0);
        return;
    } // IF IRINA CHECKS ALREADY I CAN TAKE OUT THE ELSE IF
    else if (cmd.args.size() == 2)
    {
        if (channel->getTopicMode() && !user.isOperator())
        {
            reply = user.getNick() + " :Operator permission required\n";
            send(user.getFd(), reply.c_str(), reply.length(), 0);
            return;
        }
        channel->setTopic(cmd.args[1]);
        reply = "#" + cmd.args[0] + " NEW TOPIC: " + cmd.args[1] + "\n";
        channel->broadcast(reply);
        return;
    }
}

void Server::cmdMode(User& user, const ParsedCommand& cmd);
void Server::cmdJoin(User& user, const ParsedCommand& cmd);
void Server::cmdPrivmsg(User& user, const ParsedCommand& cmd);
void Server::cmdNick(User& user, const ParsedCommand& cmd);
void Server::cmdUser(User& user, const ParsedCommand& cmd);
void Server::cmdPart(User& user, const ParsedCommand& cmd);
