#include "../../includes/Server.hpp"

/* Add a new fd to the std::vector<struct pollfd> _pfds; */
void Server::addToTheRoom(int fd)
{
    if(_pfds.size() >= _fdsize)
    {
        std::cerr << "Room is full! Rejecting client." << std::endl;
        close(fd);
        return;
    }

    struct pollfd new_client = makePollFds(fd, POLLIN);
    _pfds.push_back(new_client);
    std::cout << "Client added to the room. Total people: " << _pfds.size() << std::endl;
}

void Server::removeFromTheRoom(int fd)
{
    for(std::vector<struct pollfd>::iterator it = _pfds.begin(); it != _pfds.end(); ++it)
    {
        if(it->fd == fd)
        {
            std::cout << "Client " << fd << " removed from room. Total people: " << _pfds.size() - 1
                      << std::endl;
            close(fd);
            _pfds.erase(it);
            return;
        }
    }
    std::cerr << "Client " << fd << " not found in room" << std::endl;
}
