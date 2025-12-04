#include "../includes/Server.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    // TODO: Parse port and password
    // TODO: Start Server
    (void) argv;

    return 0;
}
