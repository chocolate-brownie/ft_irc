#include "../../includes/Parser.hpp"

Parser::Parser() {}

Parser::Parser(const Parser& other) { *this = other; }

Parser& Parser::operator=(const Parser& other) { (void)other; return *this; }

Parser::~Parser() {}

ParsedCommand Parser::split(const std::string& input) {
	if (input.empty())
		throw std::invalid_argument("Empty");

	ParsedCommand pc;

	size_t pos = input.find_first_not_of(" ");

	// PREFIX
	if (input[pos] == ':')
		throw std::invalid_argument("Prefix in input");
	else if (pos == std::string::npos)
		throw std::invalid_argument("Only spaces");

	std::string trimmed = input.substr(pos);
	std::stringstream ss(trimmed);

	// COMMAND
	if (!(ss >> pc.command))
		throw std::runtime_error("Failed to read data");

	// PARAMETERS
	std::string token;
	while (ss >> token) {
		if (token[0] == ':') { // trailing param starts
			std::string trailing = token.substr(1); //first word of the text
			std::string rest;
			std::getline(ss, rest); // reads everything remaining
			trailing += rest;
			if (trailing.empty())
				throw std::invalid_argument("Empty trailing parameter");
			pc.args.push_back(trailing);
			return pc;
		}
		pc.args.push_back(token);
	}

	return pc;
}

void IsValidCmd(ParsedCommand *pc) {
	std::string cmds_list[] = {
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
		"JOIN",
		"PRIVMSG",
		"NICK",
		"USER"
	};

	int yes = 0;
	size_t i;
	for (i = 0; i < sizeof(cmds_list)/sizeof(cmds_list[0]); i++) {
		if (cmds_list[i] == pc->command) {
			yes = 1;
			break;
		}
	}
	if (!yes)
		throw std::invalid_argument("Invalid command name");
	pc->cmd_code = i;
}

// TOPIC <channel> [<topic>]
// TOPIC #help :We help with C++ IRC projects
// TOPIC #strangers
void IsValid_TOPIC(ParsedCommand *pc) {
	if (pc->args.empty() || pc->args.size() > 2)
		throw std::invalid_argument("Wrong number of parameters for a topic");
	if (pc->args[0][0] != '#' || (pc->args[0][0] == '#' && pc->args[0].size() == 1))
		throw std::invalid_argument("Wrong channel for a topic");
	if (pc->args.size() == 2) {
		for (int i = 0; i < pc->args[1].size(); i++) {
			if (!std::isprint(pc->args[1][i]))
				throw std::invalid_argument("Unprintable character in topic text");
		}
	}
}

// PRIVMSG <receiver>{,<receiver>} <text to be sent>
// PRIVMSG #help :Hello, I need help
// PRIVMSG Bob :Can you review my code?
void IsValid_PRIVMSG(ParsedCommand *pc) {
	if (pc->args.empty() || pc->args.size() != 2)
		throw std::invalid_argument("Wrong number of parameters for a message");
	for (int i = 0; i < pc->args[1].size(); i++) {
		if (!std::isprint(pc->args[1][i]))
			throw std::invalid_argument("Unprintable character in message text");
	}
}

// KICK <channel> <user> [<comment>]
// KICK #42 Enrico :for being too pretty
// KICK #help smartass
void IsValid_KICK(ParsedCommand *pc) {
	if (pc->args.empty() || (pc->args.size() != 2 && pc->args.size() != 3))
		throw std::invalid_argument("Wrong number of parameters for a kick");
	if (pc->args[0][0] != '#' || (pc->args[0][0] == '#' && pc->args[0].size() == 1))
		throw std::invalid_argument("Wrong channel for a kick");
	if (pc->args[1][0] == '#')
		throw std::invalid_argument("Wrong user for a kick");
	if (pc->args.size() == 3) {
		for (int i = 0; i < pc->args[2].size(); i++) {
			if (!std::isprint(pc->args[2][i]))
				throw std::invalid_argument("Unprintable character in kick comment");
		}
	}
}

// INVITE <nickname> <channel>
// INVITE little_rat #trap
void IsValid_INVITE(ParsedCommand *pc) {
	if (pc->args.empty() || pc->args.size() != 2)
		throw std::invalid_argument("Wrong number of parameters for an invite");
	if (pc->args[0][0] == '#')
		throw std::invalid_argument("Wrong user for an invite");
	if (pc->args[1][0] != '#' || (pc->args[1][0] == '#' && pc->args[1].size() == 1))
		throw std::invalid_argument("Wrong channel for an invite");
}

// JOIN <channel> [<key>]
// JOIN #family
// JOIN #project 123456
void IsValid_JOIN(ParsedCommand *pc) {
	if (pc->args.empty() || pc->args.size() > 2)
		throw std::invalid_argument("Wrong number of parameters for a join");
	if (pc->args[0][0] != '#' || (pc->args[0][0] == '#' && pc->args[0].size() == 1))
		throw std::invalid_argument("Wrong channel for a join");
}

// NICK <nickname>
// NICK PrettyCat2001
void IsValid_NICK(ParsedCommand *pc) {
	if (pc->args.empty() || pc->args.size() != 1)
		throw std::invalid_argument("Wrong number of parameters to set a nickname");
	//nickname rules
}

ParsedCommand Parser::parse(const std::string& input) {
	ParsedCommand pc;
	pc.cmd_code = 0; // maybe extract from structure

	try {
		pc = split(input);
		IsValidCmd(&pc);
		switch (pc.cmd_code) // parsing depending on command
		{
			case KICK:
				IsValid_KICK(&pc);
				break;
			case INVITE:
				IsValid_INVITE(&pc);
				break;
			case TOPIC:
				IsValid_PRIVMSG(&pc);
				break;
			case MODE:
			case JOIN:
				IsValid_JOIN(&pc);
				break;
			case PRIVMSG:
				IsValid_TOPIC(&pc);
				break;
			case NICK:
				IsValid_NICK(&pc);
				break;
			case USER:
			default:
		}
		// maybe stock the array and size of our server's commands somewhere in main structure/class
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}

	return pc;
}
