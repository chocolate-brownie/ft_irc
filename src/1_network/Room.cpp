#include "../../includes/Server.hpp"

/* Add a new fd to the std::vector<struct pollfd> _pfds; */
void Server::addToTheRoom(int fd)
{
    if (_pfds.size() >= _fdsize)
    {
        std::cerr << "❌ Room is full! Rejecting client." << std::endl;
        close(fd);
        return;
    }

    struct pollfd new_client = makePollFds(fd, POLLIN);
    _pfds.push_back(new_client);
    std::cout << "📥 Client added to the room. Total people: " << _pfds.size()
              << std::endl;
}
