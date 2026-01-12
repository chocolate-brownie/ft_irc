#include "../../includes/Parser.hpp"

Parser::Parser() {}

Parser::Parser(const Parser& other) { *this = other; }

Parser& Parser::operator=(const Parser& other) {
    (void) other;
    return *this;
}

Parser::~Parser() {}

// Takes raw string of input and return a parsed command structure
ParsedCommand Parser::parse(const std::string& input) {
    ParsedCommand pc;
    int           cmd_code = 0;

    pc = split(input);
    IsValidCmd(&pc, &cmd_code);
    switch (cmd_code) {
        case KICK: IsValid_KICK(&pc); break;
        case INVITE: IsValid_INVITE(&pc); break;
        case TOPIC: IsValid_TOPIC(&pc); break;
        case MODE: IsValid_MODE(&pc); break;
        case JOIN: IsValid_JOIN(&pc); break;
        case PRIVMSG: IsValid_PRIVMSG(&pc); break;
        case NICK: IsValid_NICK(&pc); break;
        case USER: IsValid_USER(&pc); break;
        case PART: IsValid_PART(&pc); break;
        case PASS: IsValid_PASS(&pc); break;
    }

    return pc;
}

// Splits a raw IRC line into command and arguments
ParsedCommand Parser::split(const std::string& input) {
    if (input.empty()) throw std::invalid_argument("Empty");

    ParsedCommand pc;

    size_t pos = input.find_first_not_of(" ");

    // PREFIX
    if (pos == std::string::npos) throw std::invalid_argument("Only spaces");
    if (input[pos] == ':') throw std::invalid_argument("Prefix in input");

    std::string       trimmed = input.substr(pos);
    std::stringstream ss(trimmed);

    // COMMAND
    if (!(ss >> pc.command)) throw std::runtime_error("Failed to read data");

    // PARAMETERS
    std::string token;
    while (ss >> token) {
        if (token[0] == ':') {                      // trailing param starts
            std::string trailing = token.substr(1); // first word of the text
            std::string rest;
            std::getline(ss, rest); // reads everything remaining
            trailing += rest;
            if (trailing.empty()) throw std::invalid_argument("Empty trailing parameter");
            pc.args.push_back(trailing);
            return pc;
        }
        pc.args.push_back(token);
    }

    return pc;
}

// Validates command name
void IsValidCmd(ParsedCommand* pc, int* cmd_code) {
    std::string cmds_list[] = {
        "KICK", "INVITE", "TOPIC", "MODE", "JOIN", "PRIVMSG", "NICK", "USER", "PART", "PASS"
    };

    size_t i;
    for (i = 0; i < sizeof(cmds_list) / sizeof(cmds_list[0]); i++) {
        if (cmds_list[i] == pc->command) {
            *cmd_code = i + 1;
            return;
        }
    }
    throw std::invalid_argument("Unknown command name");
}

// Validates topic setting in a channel command
// TOPIC <channel> [<topic>]
void IsValid_TOPIC(ParsedCommand* pc) {
    // number of parameters (1 or 2)
    if (pc->args.size() < 1 || pc->args.size() > 2)
        throw std::invalid_argument("Wrong number of parameters for a topic");
    // 1st param must be channel
    if (pc->args[0].size() < 2 || pc->args[0][0] != '#')
        throw std::invalid_argument("Wrong channel for a topic");
    // test for printability if there is topic's text
    if (pc->args.size() == 2) {
        for (size_t i = 0; i < pc->args[1].size(); i++) {
            if (!std::isprint(static_cast<unsigned char>(pc->args[1][i])))
                throw std::invalid_argument("Unprintable character in topic text");
        }
    }
}

// Validates sending a private or channel message command
// PRIVMSG <receiver>{,<receiver>} <text to be sent>
void IsValid_PRIVMSG(ParsedCommand* pc) {
    // number of parameters (2)
    if (pc->args.size() != 2)
        throw std::invalid_argument("Wrong number of parameters for a message");
    // test for printability for message's text
    for (size_t i = 0; i < pc->args[1].size(); i++) {
        if (!std::isprint(static_cast<unsigned char>(pc->args[1][i])))
            throw std::invalid_argument("Unprintable character in message text");
    }
}

// Validates removing a user from a channel command
// KICK <channel> <user> [<comment>]
void IsValid_KICK(ParsedCommand* pc) {
    // number of parameters (2 or 3)
    if (pc->args.size() != 2 && pc->args.size() != 3)
        throw std::invalid_argument("Wrong number of parameters for a kick");
    // 1st param must be channel
    if (pc->args[0].size() < 2 || pc->args[0][0] != '#')
        throw std::invalid_argument("Wrong channel for a kick");
    // 2nd param must be user
    if (!pc->args[1].empty() && pc->args[1][0] == '#')
        throw std::invalid_argument("Wrong user for a kick");
    // test for printability if there is comment text for kick
    if (pc->args.size() == 3) {
        for (size_t i = 0; i < pc->args[2].size(); i++) {
            if (!std::isprint(static_cast<unsigned char>(pc->args[2][i])))
                throw std::invalid_argument("Unprintable character in kick comment");
        }
    }
}

// Validates inviting a user to a channel command
// INVITE <nickname> <channel>
void IsValid_INVITE(ParsedCommand* pc) {
    // number of parameters (2)
    if (pc->args.size() != 2)
        throw std::invalid_argument("Wrong number of parameters for an invite");
    // 1st param must be nickname
    if (!pc->args[0].empty() && pc->args[0][0] == '#')
        throw std::invalid_argument("Wrong user for an invite");
    // 2nd param must be channel
    if (pc->args[1].size() < 2 || pc->args[1][0] != '#')
        throw std::invalid_argument("Wrong channel for an invite");
}

// Validates joining a channel command
// JOIN <channel> [<key>]
void IsValid_JOIN(ParsedCommand* pc) {
    // number of parameters
    if (pc->args.size() < 1 || pc->args.size() > 2)
        throw std::invalid_argument("Wrong number of parameters for a join");
    // 1st param must be channel
    if (pc->args[0].size() < 2 || pc->args[0][0] != '#')
        throw std::invalid_argument("Wrong channel for a join");
}

// Validates setting or changing a nickname command
// NICK <nickname>
void IsValid_NICK(ParsedCommand* pc) {
    // number of parameters
    if (pc->args.empty() || pc->args.size() != 1)
        throw std::invalid_argument("Wrong number of parameters to set a nickname");
    // nickname length
    const std::string& nick = pc->args[0];
    if (nick.size() < 1 || nick.size() > 9)
        throw std::invalid_argument("Nickname length must be 1-9 characters");
    // check first character
    char first = nick[0];
    if (!std::isalpha(first) && first != '[' && first != ']' && first != '\\' && first != '`' &&
        first != '^' && first != '{' && first != '}' && first != '_')
        throw std::invalid_argument("Invalid first character in nickname");
    // check remaining characters
    for (size_t i = 1; i < nick.size(); ++i) {
        char c = nick[i];
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '[' && c != ']' && c != '\\' &&
            c != '`' && c != '^' && c != '{' && c != '}' && c != '_' && c != '-')
            throw std::invalid_argument("Invalid character in nickname");
    }
}

// Validates channel mode change command
// MODE <channel> {[+|-]|i|t|k|o|l} [<limit>/<password>/<user>]
void IsValid_MODE(ParsedCommand* pc) {
    // number of parameters
    if (pc->args.size() < 2 || pc->args.size() > 3)
        throw std::invalid_argument("Wrong number of parameters for MODE");
    // 1st param must be channel
    if (pc->args[0].size() < 2 || pc->args[0][0] != '#')
        throw std::invalid_argument("Wrong channel for MODE");
    // 1st param must be composed of +/- and mode flag
    const std::string& modeStr = pc->args[1];
    if (modeStr.size() != 2) throw std::invalid_argument("Invalid mode format");
    char sign = modeStr[0];
    char mode = modeStr[1];
    if (sign != '+' && sign != '-') throw std::invalid_argument("Invalid mode sign");
    // mode flag can be only i/t/k/o/l
    if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'o' && mode != 'l')
        throw std::invalid_argument("Unsupported mode");
    // modes without parameters
    if (mode == 'i' || mode == 't') {
        if (pc->args.size() != 2) throw std::invalid_argument("Mode does not take a parameter");
        return;
    }
    // modes with parameters only on '+'
    if (sign == '-') {
        if (pc->args.size() != 2)
            throw std::invalid_argument("Mode does not take a parameter when disabled");
        return;
    }
    // +k +o +l → parameter required
    if (pc->args.size() != 3) throw std::invalid_argument("Mode requires a parameter");
    // param check for k/o/l
    const std::string& param = pc->args[2];
    if (mode == 'k') {
        if (param.empty()) throw std::invalid_argument("Channel key cannot be empty");
    } else if (mode == 'o') {
        if (param.empty() || param[0] == '#')
            throw std::invalid_argument("Invalid nickname for operator mode");
    } else if (mode == 'l') {
        for (size_t i = 0; i < param.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(param[i])))
                throw std::invalid_argument("User limit must be numeric");
        }
    }
}

// Validates user registration command
// USER <username> <hostname> <servername> <realname>
void IsValid_USER(ParsedCommand* pc) {
    // number of parameters
    if (pc->args.size() != 4) throw std::invalid_argument("Wrong number of parameters for USER");

    const std::string& username   = pc->args[0];
    const std::string& hostname   = pc->args[1];
    const std::string& servername = pc->args[2];
    const std::string& realname   = pc->args[3];
    // username check
    if (username.empty()) throw std::invalid_argument("Username cannot be empty");
    for (size_t i = 0; i < username.size(); ++i)
        if (!std::isalnum(static_cast<unsigned char>(username[i])) && username[i] != '-' &&
            username[i] != '_')
            throw std::invalid_argument("Invalid character in username");
    // hostname check
    for (size_t i = 0; i < hostname.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(hostname[i])))
            throw std::invalid_argument("Invalid character in hostname");
    // servername check
    for (size_t i = 0; i < servername.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(servername[i])))
            throw std::invalid_argument("Invalid character in servername");
    // realname check
    if (realname.empty()) throw std::invalid_argument("Realname cannot be empty");
}

// Validates leaving a channel command
// PART <channel>{,<channel>}
void IsValid_PART(ParsedCommand* pc) {
    // number of parameters
    if (pc->args.size() != 1) throw std::invalid_argument("Wrong number of parameters for PART");
    // param must be channel
    if (pc->args[0].size() < 2 || pc->args[0][0] != '#')
        throw std::invalid_argument("Wrong channel for PART");
}

void IsValid_PASS(ParsedCommand* pc) {
    if (pc->args.size() != 1) {
        throw std::invalid_argument("Wrong number of parameters for PASS");
    }
}
