#include "../../includes/Parser.hpp"

Parser::Parser() {}

Parser::Parser(const Parser& other)
{
    *this = other;
}

Parser& Parser::operator=(const Parser& other)
{
    (void)other;
    return *this;
}

Parser::~Parser() {}

ParsedCommand Parser::split(const std::string& input)
{
    if (input.empty())
        throw std::invalid_argument("Empty");

    ParsedCommand pc;

    size_t pos = input.find_first_not_of(" ");

    // PREFIX
    if (input[pos] == ':')
        throw std::invalid_argument("Prefix in input");

    std::stringstream ss(input);

    // COMMAND
    if (!(ss >> pc.command))
        throw std::runtime_error("Failed to read data");

    // PARAMETERS
    std::string token;
    while (ss >> token)
    {
        if (token[0] == ':')
        {                                           // trailing param starts
            std::string trailing = token.substr(1); // remove ':'
            std::string next;
            while (ss >> next)
            {
                trailing += " " + next;
            }
            pc.args.push_back(trailing);
            return pc; // trailing must be the last param
        }
        pc.args.push_back(token);
    }

    return pc;
}

void IsValidCmd(ParsedCommand* pc)
{
    std::string cmds_list[] = {"KICK", "INVITE",  "TOPIC", "MODE",
                               "JOIN", "PRIVMSG", "NICK",  "USER"};

    int yes = 0;
    size_t i;
    for (i = 0; i < sizeof(cmds_list) / sizeof(cmds_list[0]); i++)
    {
        if (cmds_list[i] == pc->command)
        {
            yes = 1;
            break;
        }
    }
    if (!yes)
        throw std::invalid_argument("Invalid command name");
    pc->cmd_code = i;
}

// PRIVMSG #help :Hello, I need help
// PRIVMSG Bob :Can you review my code?
// TOPIC #help :We help with C++ IRC projects
void IsValid_PRIVMSG_TOPIC(ParsedCommand* pc, int msg)
{
    if (pc->args.empty() || pc->args.size() != 2)
        throw std::invalid_argument("Wrong number of parameters");
    if (pc->args[0][0] == '#' && msg) // private msg to a channel
        pc->IsPrMsgtoChannel = true;
    else if (pc->args[0][0] != '#' && !msg) // topic without channel
        throw std::invalid_argument("Wrong channel");
    if (pc->args[1].empty())
        throw std::invalid_argument("No text");
    for (std::string::size_type i = 0; i < pc->args[1].size(); i++)
    {
        if (!std::isprint(pc->args[1][i]))
            throw std::invalid_argument("Unprintable character in text");
    }
}

ParsedCommand Parser::parse(const std::string& input)
{
    ParsedCommand pc;
    pc.IsPrMsgtoChannel = false;
    pc.cmd_code = 0;

    try
    {
        pc = split(input);
        IsValidCmd(&pc);
        switch (pc.cmd_code) // parsing depending on command
        {
            case KICK:
            case INVITE:
            case TOPIC:
                IsValid_PRIVMSG_TOPIC(&pc, 0);
                break;
            case MODE:
            case JOIN:
            case PRIVMSG:
                IsValid_PRIVMSG_TOPIC(&pc, 1);
                break;
            case NICK:
            case USER:
            default:
                break;
        }
        // maybe stock the array and size of our server's commands somewhere in
        // main structure/class
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return pc;
}
