#include "../../includes/Server.hpp"

/* accept() the new client give him an fd(accept() returns a new fd) and push that fd in to the
 * room (aka vector) with other existing clients */
void Server::handleNewConnection()
{
    struct sockaddr_storage remoteaddr; // connector's address info
    int                     newfd;      // Newly accepte()ed socket fd
    socklen_t               addrlen;

    addrlen = sizeof(remoteaddr);
    if((newfd = accept(_listener, (struct sockaddr*) &remoteaddr, &addrlen)) == -1)
        std::cerr << "accpet" << std::endl;
    else
    {
        std::cout << "✅ New connection from " << getClientIP(remoteaddr) << " on socket " << newfd
                  << std::endl;
        addToTheRoom(newfd);
    }
}

bool Server::handleClientData(int client_fd)
{
    char buf[256];

    int nbytes = recv(client_fd, buf, sizeof(buf), 0); // 1. READ

    // 1. Check health of the recv()
    if(nbytes <= 0)
    {
        if(nbytes == 0) // Connection lost
            std::cout << "pollserver: socket " << client_fd << " hung up" << std::endl;
        else
            perror("recv");

        removeFromTheRoom(client_fd); // remove the dead connection from the room
        return false;
    }

    // 3. PROCESS (Success)
    buf[nbytes] = '\0';
    std::cout << "Received: " << buf << std::endl;
    return true; // They are alive keep i as it is
}

void Server::processConnections()
{
    for(size_t i = 0; i < _pfds.size(); ++i)
    {
        struct pollfd& current = _pfds[i];
        if(!(current.revents & (POLLIN | POLLHUP | POLLERR))) continue; // Check events

        /* If the _listener rings, its a new user, give him a new fd and push him to the room
         * with the other clients */
        if(current.fd == _listener)
        {
            handleNewConnection();
            continue;
        }

        /* If the connection is not coming through the main door aka _listener its an already
         * existing client trying to communicate, so deal with him */
        if(!handleClientData(current.fd)) i--;
    }
}
