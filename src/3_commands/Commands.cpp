#include "../../includes/Server.hpp"

std::string intToString(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

int StringToInt(std::string str) {
    std::stringstream ss(str);
    int               res;
    ss >> res;
    return (res);
}

std::string Server::getWelcomeMsg(User& user) {
    std::string serverName = "ft_irc.42.fr";
    std::string nick       = ((user.getNick().empty()) ? "*" : user.getNick());
    std::string username   = user.getUsername();

    std::string msg = ":" + serverName + " 001 " + nick +
                      " :Welcome to the Internet Relay Network " + nick + "!" + username + "@" +
                      user.getHostname() + "\r\n";
    msg += ":" + serverName + " 002 " + nick + " :Your host is " + serverName +
           ", running version 1.0\r\n";
    msg += ":" + serverName + " 003 " + nick + " :This server was created " + this->getStartTime() +
           "\r\n";
    msg += ":" + serverName + " 004 " + nick + " " + serverName + " 1.0 o iklt\r\n";

    return (msg);
}

void Server::reply(int id, User& user, std::string arg1, std::string arg2) {
    // Build the Prefix: :ServerName <Code> <ClientNick> : <arg> /r/n
    // :BBCServer 404 Mike : blablabla\r\n
    //  Note: If user is not registered (Nick is empty), standard uses "*"
    std::string user_nick = ((user.getNick().empty()) ? "*" : user.getNick());
    std::string msg       = ":ft_irc.42.fr " + intToString(id) + " " + user_nick + " ";
    switch (id) {
        case RPL_WELCOME: msg = getWelcomeMsg(user); break;
        case RPL_NOTOPIC: msg += arg1 + " :No topic is set\r\n"; break;
        case RPL_TOPIC:
            // Arg1 = channel name, Arg2 = topic
            msg += arg1 + " :" + arg2 + "\r\n";
            break;
        case RPL_INVITING:
            // arg1 = Target Nick, arg2 = Channel Name
            msg += arg1 + " " + arg2 + "\r\n";
            break;
        case RPL_NAMREPLY:
            // Arg1 = channel name, Arg2 = list of names
            msg += "= " + arg1 + " :" + arg2 + "\r\n";
            break;
        case RPL_ENDOFNAMES: msg += arg1 + " :End of /NAMES list\r\n"; break;
        case ERR_NOORIGIN: msg += ":No origin specified\r\n"; break;
        case ERR_NOSUCHNICK: msg += arg1 + " :No such nick/channel\r\n"; break;
        case ERR_NOSUCHCHANNEL: msg += arg1 + " :No such channel\r\n"; break;
        case ERR_CANNOTSENDTOCHAN: msg += arg1 + " :Cannot send to channel\r\n"; break;
        case ERR_UNKNOWNCOMMAND: msg += arg1 + " :Unknown command\r\n"; break;
        case ERR_NICKNAMEINUSE: msg += arg1 + " :Nickname is already in use\r\n"; break;
        case ERR_USERNOTINCHANNEL: msg += arg1 + " :They aren't on that channel\r\n"; break;
        case ERR_NOTONCHANNEL: msg += arg1 + " :You're not on that channel\r\n"; break;
        case ERR_USERONCHANNEL: msg += arg1 + " :is already on channel\r\n"; break;
        case ERR_NOTREGISTERED: msg += " :You have not registered\r\n"; break;
        case ERR_NEEDMOREPARAMS: msg += arg1 + " :Not enough parameters\r\n"; break;
        case ERR_ALREADYREGISTRED: msg += " :You're already registered\r\n"; break;
        case ERR_PASSWDMISMATCH: msg += arg1 + " :Password incorrect\r\n"; break;
        case ERR_CHANNELISFULL: msg += arg1 + " :Channel is full (+l)\r\n"; break;
        case ERR_UNKNOWNMODE:
            // arg1 = the unexpected char
            msg += arg1 + " :is unknown mode char to me\r\n";
            break;
        case ERR_INVITEONLYCHAN: msg += arg1 + " :Invite needed to join (+i)\r\n"; break;
        case ERR_BADCHANNELKEY: msg += arg1 + " :Wrong channel key (+k)\r\n"; break;
        case ERR_CHANOPRIVSNEEDED: msg += arg1 + " :You're not channel operator\r\n"; break;
        default: msg += " :Unknown error\r\n"; break;
    }
    send(user.getFd(), msg.c_str(), msg.length(), 0);
}

void Server::addChannel(Channel* channel) { _channels.push_back(channel); }

void Server::rmvChannel(Channel* channel) {
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
        if ((*it) == channel) {
            delete *it;
            _channels.erase(it);
            return;
        }
    }
}

// Returns a Channel from the vector given its name
Channel* Server::getChannel(const std::string& name) {
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++) {
        if ((*it)->getName() == name) return (*it);
    }
    return (NULL);
}

User* Server::getUser(const std::string& name) {
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second->getNick() == name) { return it->second; }
    }
    return NULL;
}

void Server::executeCommand(User& user, const ParsedCommand& cmd) {
    if (!user.isRegistered()) {
        bool isLoginCmd = (cmd.cmd == PASS || cmd.cmd == NICK || cmd.cmd == USER);
        if (!isLoginCmd) {
            this->reply(ERR_NOTREGISTERED, user, "", "");
            return;
        }
    }

    switch (cmd.cmd) {
        case KICK: cmdKick(user, cmd); break;
        case INVITE: cmdInvite(user, cmd); break;
        case TOPIC: cmdTopic(user, cmd); break;
        case MODE: cmdMode(user, cmd); break;
        case JOIN: cmdJoin(user, cmd); break;
        case PRIVMSG: cmdPrivmsg(user, cmd); break;
        case NICK: cmdNick(user, cmd); break;
        case USER: cmdUser(user, cmd); break;
        case PART: cmdPart(user, cmd); break;
        case PASS: cmdPass(user, cmd); break;
        case QUIT: cmdQuit(user, cmd); break;
        case CAP: return;
        case WHO: cmdWho(user, cmd); break;
        case PING: cmdPing(user, cmd); break;
        default: this->reply(ERR_UNKNOWNCOMMAND, user, cmd.command, ""); break;
    }
}

void Server::cmdKick(User& user, const ParsedCommand& cmd) {
    Channel*    channel;
    User*       target;
    std::string reply;
    std::string reason =
        (cmd.args.size() > 2 && !cmd.args[2].empty()) ? cmd.args[2] : "Kicked by operator";

    // Checks if channel exists, if user is operator and if the target user is
    // in the channel
    if (!(channel = this->getChannel(cmd.args[0]))) {
        this->reply(ERR_NOSUCHCHANNEL, user, cmd.args[0], "");
        return;
    }
    if (!channel->isOperator(user)) {
        this->reply(ERR_CHANOPRIVSNEEDED, user, user.getNick(), "");
        return;
    }
    if (!(target = channel->isUserConnected(cmd.args[1]))) {
        this->reply(ERR_USERNOTINCHANNEL, user, cmd.args[1] + " " + channel->getName(), "");
        return;
    }
    // When kicking user we remove him from the channel list and vice versa we
    // remove the channel from his list, and broadcast the message to inform the
    // channel

    reply = ":" + user.getPrefix() + " KICK " + channel->getName() + " " + target->getNick() +
            " :" + reason + "\r\n";
    channel->broadcast(reply);
    channel->removeUser(*target);
    target->removeChannel(*channel);
}

void Server::cmdInvite(User& user, const ParsedCommand& cmd) {
    Channel*    channel;
    User*       target;
    std::string invite;

    // Checks if channel exists and if the target user is
    // not already in the channel, then
    // IF INVITE MODE TRUE and USER IS NOT OPERATOR
    // we send the error otherwise we invite the target
    if (!(channel = this->getChannel(cmd.args[1]))) {
        this->reply(ERR_NOSUCHCHANNEL, user, cmd.args[1], "");
        return;
    }
    if (!(target = this->getUser(cmd.args[0]))) {
        this->reply(ERR_NOSUCHNICK, user, cmd.args[0], "");
        return;
    }
    if ((channel->isUserConnected(*target))) {
        this->reply(ERR_USERONCHANNEL, user, cmd.args[0], "");
        return;
    }
    if (!(channel->isUserConnected(user.getNick()))) {
        this->reply(ERR_NOTONCHANNEL, user, channel->getName(), "");
        return;
    }
    if (channel->getInviteMode() && !(channel->isOperator(user))) {
        this->reply(ERR_CHANOPRIVSNEEDED, user, channel->getName(), "");
        return;
    }

    // We send the invite
    channel->addInvited(*target);
    invite = ":" + user.getPrefix() + " INVITE " + target->getNick() + " :" + channel->getName() +
             "\r\n";
    send((*target).getFd(), invite.c_str(), invite.length(), 0);
    // We send confimation to the user
    this->reply(RPL_INVITING, user, target->getNick(), channel->getName());
}

void Server::cmdTopic(User& user, const ParsedCommand& cmd) {
    Channel*    channel;
    std::string reply;
    if (!(channel = this->getChannel(cmd.args[0]))) {
        this->reply(ERR_NOSUCHCHANNEL, user, cmd.args[0], "");
        return;
    }
    if (cmd.args.size() == 1) {
        if (channel->getTopic().empty()) this->reply(RPL_NOTOPIC, user, channel->getName(), "");
        else
            this->reply(RPL_TOPIC, user, channel->getName(), channel->getTopic());
        return;
    } // IF IRINA CHECKS ALREADY I CAN TAKE OUT THE ELSE IF
    else if (cmd.args.size() == 2) {
        if (channel->getTopicMode() && !channel->isOperator(user)) {
            this->reply(ERR_CHANOPRIVSNEEDED, user, channel->getName(), "");
            return;
        }
        channel->setTopic(cmd.args[1]);
        reply =
            ":" + user.getPrefix() + " TOPIC " + channel->getName() + " :" + cmd.args[1] + "\r\n";
        channel->broadcast(reply);
        return;
    }
}

void Server::cmdMode(User& user, const ParsedCommand& cmd) {
    Channel* channel;
    if (!(channel = this->getChannel(cmd.args[0]))) {
        this->reply(ERR_NOSUCHCHANNEL, user, cmd.args[0], "");
        return;
    }
    if (!channel->isOperator(user) && cmd.args.size() > 1) {
        this->reply(ERR_CHANOPRIVSNEEDED, user, channel->getName(), "");
        return;
    }

    // state indicates whether we are adding a flag or removing it
    // arg_index points at the next argument out flags refer to
    std::string flags     = (cmd.args.size() > 1) ? cmd.args[1] : "";
    std::string changes   = "";
    std::string log       = "";
    bool        state     = true;
    int         arg_index = 2;
    char        c;
    for (size_t i = 0; i < flags.length(); i++) {
        c = flags[i];
        switch (c) {
            case '+': state = true; break;
            case '-': state = false; break;
            case 'i':
                channel->setInviteMode(state);
                changes += (state ? "+i" : "-i");
                break;
            case 't':
                channel->setTopicMode(state);
                changes += (state ? "+t" : "-t");
                break;
            case 'k':
                if (state) {
                    if ((size_t) arg_index >= cmd.args.size()) {
                        this->reply(ERR_NEEDMOREPARAMS, user, "MODE", "");
                        continue;
                    }
                    channel->setKey(cmd.args[arg_index]);
                    channel->setKeyMode(true);
                    changes += "+k";
                    log += cmd.args[arg_index++] + " ";
                } else {
                    channel->setKeyMode(false);
                    changes += "-k";
                }
                break;
            case 'o':
                if (arg_index >= (int) cmd.args.size()) {
                    this->reply(ERR_NEEDMOREPARAMS, user, "MODE", "");
                    continue;
                }
                User* target;
                if (!(target = this->getUser(cmd.args[arg_index]))) {
                    this->reply(ERR_NOSUCHNICK, user, cmd.args[arg_index], "");
                    arg_index++;
                    continue;
                }
                if (!channel->isUserConnected(*target)) {
                    this->reply(ERR_USERNOTINCHANNEL, user, cmd.args[arg_index], "");
                    arg_index++;
                    continue;
                }
                if (state) {
                    if (!channel->isUserConnected(*target)) channel->addUser(*target);
                    channel->addOperator(*target);
                    changes += "+o";
                } else {
                    channel->removeOperator(*target);
                    changes += "-o";
                }
                log += cmd.args[arg_index++] + " ";
                break;
            case 'l':
                if (state) {
                    int limit = StringToInt(cmd.args[arg_index]);
                    if (limit <= 0) continue;
                    channel->setLimit(limit);
                    changes += "+l";
                    log += cmd.args[arg_index++] + " ";
                } else
                    changes += "-l";
                channel->setLimitMode(state);
                break;
            default: this->reply(ERR_UNKNOWNMODE, user, std::string(1, c), ""); return;
        }
    }
    if (!changes.empty()) {
        std::string mode_msg = ":" + user.getPrefix() + " MODE " + channel->getName() + " " +
                               changes + " " + log + "\r\n";
        channel->broadcast(mode_msg);
    } else {
        std::string msg = ":ft_irc.42.fr 324 " + user.getNick() + " " + channel->getMode() + "\r\n";
        send(user.getFd(), msg.c_str(), msg.size(), 0);
    }
}

void Server::cmdJoin(User& user, const ParsedCommand& cmd) {
    Channel*    channel;
    std::string reply;
    std::string provided_key = (cmd.args.size() > 1) ? cmd.args[1] : "";

    std::string channelName = cmd.args[0];
    if (channelName[0] != '#') { channelName = "#" + channelName; }
    // Check if channel exists and if user is already connected (in this case we
    // just ignore him)
    if (!(channel = this->getChannel(channelName))) {
        channel = new Channel(channelName);
        this->addChannel(channel);
        channel->addOperator(user);
    } else {
        if (channel->isUserConnected(user)) return;
        // When invite mode is active, if user is invited he can join else no
        if (channel->getInviteMode()) {
            if (!channel->isInvited(user)) {
                this->reply(ERR_INVITEONLYCHAN, user, channel->getName(), "");
                return;
            }
            channel->removeInvited(user);
        } // When key mode is active we check if the given password matches
        else if (channel->getKeyMode() && provided_key.compare(channel->getKey()) != 0 &&
                 !channel->isInvited(user)) {
            this->reply(ERR_BADCHANNELKEY, user, channel->getName(), "");
            return;
        } // When limit mode is active we check if the channel isn't full
        if (channel->getLimitMode() && channel->getNumberUsers() >= channel->getLimit()) {
            this->reply(ERR_CHANNELISFULL, user, channel->getName(), "");
            return;
        }
    }
    // To join we add the user to the channel list and viceversa and we inform
    // the channel users
    channel->addUser(user);
    user.addChannel(*channel);
    reply = ":" + user.getPrefix() + " JOIN :" + channel->getName() + "\r\n";
    channel->broadcast(reply);
    // We also send the channel topic and list of users in the channel to the
    // user
    if (!channel->getTopic().empty())
        this->reply(RPL_TOPIC, user, channel->getName(), channel->getTopic());
    this->reply(RPL_NAMREPLY, user, channel->getName(), channel->getUserList());
    this->reply(RPL_ENDOFNAMES, user, channel->getName(), "");
}

void Server::cmdPrivmsg(User& user, const ParsedCommand& cmd) {
    std::string msg;

    // Distinguish between #channel and user
    if (cmd.args[0][0] == '#') {
        Channel* channel;
        if (!(channel = this->getChannel(cmd.args[0]))) {
            this->reply(ERR_NOSUCHCHANNEL, user, cmd.args[0], "");
            return;
        }
        if (!channel->isUserConnected(user)) {
            this->reply(ERR_CANNOTSENDTOCHAN, user, channel->getName(), "");
            return;
        }
        msg =
            ":" + user.getPrefix() + " PRIVMSG " + channel->getName() + " :" + cmd.args[1] + "\r\n";

        channel->broadcast(user, msg);
        return;
    }
    User* target;
    if (!(target = this->getUser(cmd.args[0]))) {
        this->reply(ERR_NOSUCHNICK, user, cmd.args[0], "");
        return;
    }
    msg = ":" + user.getPrefix() + " PRIVMSG " + target->getNick() + " :" + cmd.args[1] + "\r\n";

    send(target->getFd(), msg.c_str(), msg.size(), 0);
}

void Server::cmdNick(User& user, const ParsedCommand& cmd) {
    if (cmd.args.empty() || cmd.args[0].empty()) {
        this->reply(ERR_NONICKNAMEGIVEN, user, "", "");
        return;
    }

    if (this->getUser(cmd.args[0])) {
        this->reply(ERR_NICKNAMEINUSE, user, cmd.args[0], "");
        if (!user.isRegistered()) { _clients_to_disconnect.push_back(user.getFd()); }
        return;
    }

    std::string reply = ":" + user.getPrefix() + " NICK :" + cmd.args[0] + "\r\n";
    user.setNick(cmd.args[0]);

    if (!user.isRegistered() && !user.getUsername().empty()) {
        if (user.isPassGiven()) {
            user.setRegistered(true);
            this->reply(RPL_WELCOME, user, "", "");
        } else {
            _clients_to_disconnect.push_back(user.getFd());
        }
        return;
    }

    send(user.getFd(), reply.c_str(), reply.length(), 0);
    std::vector<Channel*> user_channels = user.getChannels();
    for (size_t i = 0; i < user_channels.size(); i++) user_channels[i]->broadcast(user, reply);
}

void Server::cmdUser(User& user, const ParsedCommand& cmd) {
    // The USER command requires 4 arguments.
    if (cmd.args.size() < 4) {
        this->reply(ERR_NEEDMOREPARAMS, user, cmd.command, "");
        // Disconnect an unregistered user for bad format
        if (!user.isRegistered()) { _clients_to_disconnect.push_back(user.getFd()); }
        return;
    }

    if (user.isRegistered()) {
        this->reply(ERR_ALREADYREGISTRED, user, "", "");
        return;
    }

    user.setUsername(cmd.args[0]);
    user.setRealname(cmd.args[3]);

    if (!user.getNick().empty()) {
        if (user.isPassGiven()) {
            user.setRegistered(true);
            this->reply(RPL_WELCOME, user, "", "");
        } else {
            _clients_to_disconnect.push_back(user.getFd());
        }
    }
}

void Server::cmdPart(User& user, const ParsedCommand& cmd) {
    Channel* channel;

    // We check if channel exist and if user is connected
    if (!(channel = this->getChannel(cmd.args[0]))) {
        this->reply(ERR_NOSUCHCHANNEL, user, cmd.args[0], "");
        return;
    }
    if (!channel->isUserConnected(user)) {
        this->reply(ERR_NOTONCHANNEL, user, cmd.args[0], "");
        return;
    }
    std::string reason = (cmd.args.size() > 1) ? cmd.args[1] : "Leaving";
    std::string reply  = ":" + user.getPrefix() + " PART " + cmd.args[0] + " :" + reason + "\r\n";
    channel->broadcast(reply);

    channel->removeUser(user);
    user.removeChannel(*channel);

    // If the channel has no user left we delete it
    std::string list = channel->getUserList();
    if (list.empty()) this->rmvChannel(channel);
}

void Server::cmdPass(User& user, const ParsedCommand& cmd) {
    if (user.isRegistered()) {
        this->reply(ERR_ALREADYREGISTRED, user, "", "");
        return;
    }
    if (cmd.args[0].compare(_password) != 0) {
        this->reply(ERR_PASSWDMISMATCH, user, "", "");
        return;
    }
    user.setPassGiven(true);
}

void Server::cmdQuit(User& user, const ParsedCommand& cmd) {
    std::string reason   = cmd.args.empty() ? "Client Quit" : cmd.args[0];
    std::string quit_msg = ":" + user.getPrefix() + " QUIT :" + reason + "\r\n";

    std::vector<Channel*> channels = user.getChannels();
    for (size_t i = 0; i < channels.size(); i++) { channels[i]->broadcast(user, quit_msg); }

    user.quitAllChannels();

    // If user left a channel empty we make sure to delete it
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->getNumberUsers() == 0) { this->rmvChannel(channels[i]); }
    }

    _clients_to_disconnect.push_back(user.getFd());
}

void Server::cmdWho(User& user, const ParsedCommand& cmd) {
    std::string target = cmd.args[0];

    if (target[0] == '#') {
        Channel* channel = this->getChannel(target);
        if (channel) {
            std::vector<User*> users = channel->getUsers();

            for (size_t i = 0; i < users.size(); ++i) {
                User* u = users[i];

                // - Flags -
                // H = Here, G = Gone
                // @ = Channel Operator
                std::string flags = "H";
                if (channel->isOperator(*u)) flags += "@";

                // Format: <channel> <username> <host> <server> <nick> <flags> :<hopcount>
                // <realname>
                // Example: #test guest 127.0.0.1 ft_irc.42.fr guest H@ :0 Real Name
                std::string msg = ":" + user.getHostname() + " 352 " + user.getNick() + " " +
                                  target + " " + u->getUsername() + " " + u->getHostname() + " " +
                                  "ft_irc.42.fr " + u->getNick() + " " + flags + " :0 " +
                                  u->getRealname() + "\r\n";

                send(user.getFd(), msg.c_str(), msg.length(), 0);
            }
        }
    }
    // send End Of /WHO list
    std::string endMsg = ":" + user.getHostname() + " 315 " + user.getNick() + " " + target +
                         " :End of /WHO list.\r\n";
    send(user.getFd(), endMsg.c_str(), endMsg.length(), 0);
}

void Server::cmdPing(User& user, const ParsedCommand& cmd) {
    if (cmd.args.empty()) {
        this->reply(ERR_NOORIGIN, user, "", "");
        return;
    }
    std::string reply =
        ":" + user.getHostname() + " PONG " + user.getHostname() + " :" + cmd.args[0] + "\r\n";

    send(user.getFd(), reply.c_str(), reply.length(), 0);
}
