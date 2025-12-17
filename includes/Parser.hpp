#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <exception>

#define KICK 1
#define INVITE 2
#define TOPIC 3
#define MODE 4
#define JOIN 5
#define PRIVMSG 6
#define NICK 7
#define USER 8
#define PART 9

struct ParsedCommand {
	std::string command;              // e.g., "JOIN"
	std::vector<std::string> args;    // e.g., {"#42", "mypassword"}
};

class Parser {
	private:
		Parser();
		Parser(const Parser& other);
		Parser& operator=(const Parser& other);
		~Parser();

		static ParsedCommand split(const std::string& input);
	public:
		// The Input is a raw string. The Output is a structured object.
		static ParsedCommand parse(const std::string& input);
};
