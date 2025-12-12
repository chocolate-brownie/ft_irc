#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include <string>#include <vector>#include <iostream>// A simple struct to hold the result of parsing
struct ParsedCommand {
    std::string command;              // e.g., "JOIN"
    std::vector<std::string> args;    // e.g., {"#42w", "mypassword"}
    bool isValid;                     // Did it parse correctly?
};

class Parser {
public:
    // The Input is a raw string. The Output is a structured object.
    static ParsedCommand parse(const std::string &input);
};

#endif