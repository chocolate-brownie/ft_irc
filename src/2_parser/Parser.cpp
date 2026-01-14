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

    pc = split(input);
	if (pc.err) return pc; // id err set to ERR return
    IsValidCmd(&pc);
    switch (pc.cmd) {
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
		default: break;
    }
    return pc;
}

// Splits a raw IRC line into command and arguments
ParsedCommand Parser::split(const std::string& input) {
    ParsedCommand pc;
    pc.err = 0;
    pc.cmd = 0;
	if (input.empty()) { pc.err = ERR_UNKNOWNCOMMAND; return pc; } // empty
    size_t pos = input.find_first_not_of(" ");

    // PREFIX
    if (pos == std::string::npos) { pc.err = ERR_UNKNOWNCOMMAND; return pc; } //Only spaces
    if (input[pos] == ':') { pc.err = ERR_UNKNOWNCOMMAND; return pc; } // Prefix in input

    std::string       trimmed = input.substr(pos);
    std::stringstream ss(trimmed);

    // COMMAND
    if (!(ss >> pc.command)) { pc.err = ERR_UNKNOWNCOMMAND; return pc; } // Failed to read data

    // PARAMETERS
    std::string token;
    while (ss >> token) {
        if (token[0] == ':') {                      // trailing param starts
            std::string trailing = token.substr(1); // first word of the text
            std::string rest;
            std::getline(ss, rest); // reads everything remaining
            trailing += rest;
            if (trailing.empty()) { pc.err = ERR_NEEDMOREPARAMS; return pc; } // Empty trailing parameter
            pc.args.push_back(trailing);
            return pc;
        }
        pc.args.push_back(token);
    }

    return pc;
}

// Validates command name
void IsValidCmd(ParsedCommand* pc) {
    std::string cmds_list[] = {
        "KICK", "INVITE", "TOPIC", "MODE", "JOIN", "PRIVMSG", "NICK", "USER", "PART", "PASS"
    };

    size_t i;
    for (i = 0; i < sizeof(cmds_list) / sizeof(cmds_list[0]); i++) {
        if (cmds_list[i] == pc->command) {
            pc->cmd = i + 1;
            return;
        }
    }
    //throw std::invalid_argument("Unknown command name");
	pc->err = ERR_UNKNOWNCOMMAND;
	pc->cmd = -1;
}

// Validates topic setting in a channel command
// TOPIC <channel> [<topic>]
void IsValid_TOPIC(ParsedCommand* pc) {
    // number of parameters (1 or 2)
    if (pc->args.size() < 1)
        { pc->err = ERR_NEEDMOREPARAMS; return; }
    // test for printability if there is topic's text
    if (pc->args.size() == 2) {
        for (size_t i = 0; i < pc->args[1].size(); i++) {
            if (!std::isprint(static_cast<unsigned char>(pc->args[1][i])))
                { pc->err = ERR_UNKNOWNCOMMAND; return; };
        }
    }
}

// Validates sending a private or channel message command
// PRIVMSG <receiver>{,<receiver>} <text to be sent>
void IsValid_PRIVMSG(ParsedCommand* pc) {
    // number of parameters (2)
    if (pc->args.size() < 2)
        { pc->err = ERR_NEEDMOREPARAMS; return; }
    // test for printability for message's text
    for (size_t i = 0; i < pc->args[1].size(); i++) {
        if (!std::isprint(static_cast<unsigned char>(pc->args[1][i])))
            { pc->err = ERR_UNKNOWNCOMMAND; return; };
    }
}

// Validates removing a user from a channel command
// KICK <channel> <user> [<comment>]
void IsValid_KICK(ParsedCommand* pc) {
    // number of parameters (2 or 3)
    if (pc->args.size() < 2)
        { pc->err = ERR_NEEDMOREPARAMS; return; }
    // test for printability if there is comment text for kick
    if (pc->args.size() == 3) {
        for (size_t i = 0; i < pc->args[2].size(); i++) {
            if (!std::isprint(static_cast<unsigned char>(pc->args[2][i])))
                { pc->err = ERR_UNKNOWNCOMMAND; return; };
        }
    }
}

// Validates inviting a user to a channel command
// INVITE <nickname> <channel>
void IsValid_INVITE(ParsedCommand* pc) {
    // number of parameters (2)
    if (pc->args.size() < 2)
        { pc->err = ERR_NEEDMOREPARAMS; return; }
}

// Validates joining a channel command
// JOIN <channel> [<key>]
void IsValid_JOIN(ParsedCommand* pc) {
    // number of parameters (1/2)
    if (pc->args.size() < 1)
        { pc->err = ERR_NEEDMOREPARAMS; return; }
}

// Validates setting or changing a nickname command
// NICK <nickname>
void IsValid_NICK(ParsedCommand* pc) {
    // number of parameters (1)
    if (pc->args.size() < 1)
        { pc->err = ERR_NEEDMOREPARAMS; return; }
    // nickname length (must be 1-9 chars)
    const std::string& nick = pc->args[0];
    if (nick.size() < 1 || nick.size() > 9)
        { pc->err = ERR_UNKNOWNCOMMAND; return; };
    // check first character
    char first = nick[0];
    if (!std::isalpha(first) && first != '[' && first != ']' && first != '\\' && first != '`' &&
        first != '^' && first != '{' && first != '}' && first != '_')
        { pc->err = ERR_UNKNOWNCOMMAND; return; };
    // check remaining characters
    for (size_t i = 1; i < nick.size(); ++i) {
        char c = nick[i];
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '[' && c != ']' && c != '\\' &&
            c != '`' && c != '^' && c != '{' && c != '}' && c != '_' && c != '-')
            { pc->err = ERR_UNKNOWNCOMMAND; return; };
    }
}

// Validates channel mode change command
// MODE <channel> {[+|-]|i|t|k|o|l} [<limit>/<password>/<user>]
void IsValid_MODE(ParsedCommand* pc) {
    // number of parameters (2/3)
    if (pc->args.size() < 2)
        { pc->err = ERR_NEEDMOREPARAMS; return; }

    const std::string& modes = pc->args[1];
	bool state = true;
    int  needed_args = 0;
    bool has_sign = false;

    for (size_t i = 0; i < modes.size(); ++i) {
        char c = modes[i];

        if (c == '+') {
            state = true;
            has_sign = true;
        }
        else if (c == '-') {
            state = false;
            has_sign = true;
        }
        else {
            if (!has_sign) {
                pc->err = ERR_UNKNOWNMODE;
                return;
            }
            switch (c) {
                case 'i':
                case 't': break; // no arguments
                case 'k':
                case 'l': { if (state) needed_args++; break; }
                case 'o': { needed_args++; break; }
                default: { pc->err = ERR_UNKNOWNMODE; return; }
            }
        }
    }

	// Number of provided parameters after <modes>
    int provided_args = pc->args.size() - 2;

    if (provided_args < needed_args) {
        pc->err = ERR_NEEDMOREPARAMS;
        return;
    }
}

// Validates user registration command
// USER <username> <hostname> <servername> <realname>
void IsValid_USER(ParsedCommand* pc) {
    // number of parameters (4)
    if (pc->args.size() < 4) { pc->err = ERR_NEEDMOREPARAMS; return; }

    const std::string& username   = pc->args[0];
    const std::string& hostname   = pc->args[1];
    const std::string& servername = pc->args[2];
    const std::string& realname   = pc->args[3];
    // username check
    if (username.empty()) { pc->err = ERR_UNKNOWNCOMMAND; return; };
    for (size_t i = 0; i < username.size(); ++i)
        if (!std::isalnum(static_cast<unsigned char>(username[i])) && username[i] != '-' &&
            username[i] != '_')
            { pc->err = ERR_UNKNOWNCOMMAND; return; };
    // hostname check
    for (size_t i = 0; i < hostname.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(hostname[i])))
            { pc->err = ERR_UNKNOWNCOMMAND; return; };
    // servername check
    for (size_t i = 0; i < servername.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(servername[i])))
            { pc->err = ERR_UNKNOWNCOMMAND; return; };
    // realname check
    if (realname.empty()) { pc->err = ERR_UNKNOWNCOMMAND; return; };
}

// Validates leaving a channel command
// PART <channel>{,<channel>}
void IsValid_PART(ParsedCommand* pc) {
    // number of parameters (1)
    if (pc->args.size() < 1) { pc->err = ERR_NEEDMOREPARAMS; return; }
}

// Validates password
// PASS <password>
void IsValid_PASS(ParsedCommand* pc) {
    if (pc->args.size() < 1) {
        { pc->err = ERR_NEEDMOREPARAMS; return; }
    }
}
