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

void	Server::print_error(int id, User* user, Channel* channel)
{
	std::string	msg = ":BBCServer " + std::to_string(id) + " ";
	switch (id)
	{
	case ERR_NOSUCHNICK:
		msg += user->getNick() + ": user not found\n";
		break;
	case ERR_NICKALREADYUSE:
		msg += user->getNick() + ": nickname is already taken\n";
		break;
	case ERR_NOSUCHCHANNEL:
		msg += "#" + channel->getName() + ": channel not found\n";
		break;
	case ERR_USERNOTINCHANNEL:
		msg += user->getNick() + ": user not connected to the channel\n";
		break;
	case ERR_USERINCHANNEL:
		msg += user->getNick() + ": user already connected to the channel\n";
		break;
	case ERR_NOTREGISTERED:
		msg += user->getNick() + ": user not registered\n";
		break;
	case ERR_CHANNELISFULL:
		msg += "#" + channel->getName() + ": channel full\n";;
		break;
	case ERR_INVITEONLYCHAN:
		msg += "#" + channel->getName() + ": invite needed to \n";;
		break;
	case ERR_BADCHANNELKEY:
		msg += "#" + channel->getName() + ": invalid channel key\n";
		break;
	case ERR_NOPRIVILEGES:
		msg += user->getNick() + ": permission denied, operator status required\n";
		break;
	default:
		msg = "[UNKNOWN ERROR]\n";
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
		print_error(ERR_NOPRIVILEGES, &user, NULL);
        return;
    }
    if (!(channel = this->getChannel(cmd.args[0])))
    {
		print_error(ERR_NOSUCHCHANNEL, NULL, channel);
        return;
    }
    if (!(target = channel->isUserConnected(cmd.args[1])))
    {
		print_error(ERR_USERNOTINCHANNEL, target, NULL);
        return;
    }
	// When kicking user we remove him from the channel list and vice versa we
	// remove the channel from his list, and broadcast the message to inform the channel 
    reply = user.getNick() + " KICK #" + channel->getName() + " " +
            target->getNick() + "\n";
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
		print_error(ERR_NOSUCHCHANNEL, NULL, channel);
        return;
    }
    if ((target = channel->isUserConnected(cmd.args[1])))
    {
		print_error(ERR_USERNOTINCHANNEL, target, NULL);
        return;
    }
    if (!(channel->isUserConnected(user.getNick())))
    {
		print_error(ERR_USERNOTINCHANNEL, &user, NULL);
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
			print_error(ERR_NOPRIVILEGES, &user, NULL);
        return;
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
