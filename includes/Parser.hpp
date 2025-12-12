#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <exception>

struct ParsedCommand {
	std::string command;              // e.g., "JOIN"
	std::vector<std::string> args;    // e.g., {"#42", "mypassword"}
	bool isValid; // maybe no need
};

class Parser {
	private:
		Parser();
		Parser(const Parser& other);
		Parser& operator=(const Parser& other);
		~Parser();

	public:
		// The Input is a raw string. The Output is a structured object.
		static ParsedCommand parse(const std::string& input);
		static ParsedCommand split(const std::string& input);
};
