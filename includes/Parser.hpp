#pragma once

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define ERR_UNKNOWNCOMMAND 421
#define ERR_NEEDMOREPARAMS 461
#define ERR_UNKNOWNMODE 472

#define KICK 1
#define INVITE 2
#define TOPIC 3
#define MODE 4
#define JOIN 5
#define PRIVMSG 6
#define NICK 7
#define USER 8
#define PART 9
#define PASS 10
#define QUIT 11
#define CAP 12

struct ParsedCommand {
    std::string              command; // e.g., "JOIN"
    std::vector<std::string> args;    // e.g., {"#42", "mypassword"}
    int                      cmd;     // command code
    int                      err;     // 0 if correct, ERR num if not
};

void IsValidCmd(ParsedCommand* pc);
void IsValid_TOPIC(ParsedCommand* pc);
void IsValid_PRIVMSG(ParsedCommand* pc);
void IsValid_KICK(ParsedCommand* pc);
void IsValid_INVITE(ParsedCommand* pc);
void IsValid_JOIN(ParsedCommand* pc);
void IsValid_NICK(ParsedCommand* pc);
void IsValid_MODE(ParsedCommand* pc);
void IsValid_USER(ParsedCommand* pc);
void IsValid_PART(ParsedCommand* pc);
void IsValid_PASS(ParsedCommand* pc);

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
