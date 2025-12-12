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

	std::stringstream ss(input);

	// COMMAND
	if (!(ss >> pc.command))
		throw std::runtime_error("Failed to read data");

	// PARAMETERS
	std::string token;
	while (ss >> token) {
		if (token[0] == ':') { // trailing param starts
			std::string trailing = token.substr(1);  // remove ':'
			std::string next;
			while (ss >> next) {
				trailing += " " + next;
			}
			pc.args.push_back(trailing);
			return pc; //trailing must be the last param
		}
		pc.args.push_back(token);
	}

	return pc;
}

void IsValidCmd(std::string cmd) {
	std::string cmds_list[] = {
		"KICK",
		"INVITE",
		"TOPIC",
		"MODE",
		"JOIN",
		"QUIT",
		"PRIVMSG",
		"PASS"
	};

	int yes = 0;
	for (int i = 0; i < sizeof(cmds_list)/sizeof(cmds_list[0]); i++) {
		if (cmds_list[i] == cmd) {
			yes = 1;
			break;
		}
	}
	if (!yes)
		throw std::invalid_argument("Invalid command");
}

ParsedCommand Parser::parse(const std::string& input) {
	ParsedCommand pc;

	try {
		pc = split(input);
		IsValidCmd(pc.command);
		// parsing depending on command
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}

	return pc;
}
