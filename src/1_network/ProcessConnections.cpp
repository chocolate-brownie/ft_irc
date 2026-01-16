#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/User.hpp"

/* accept() the new client give him an fd(accept() returns a new fd) and push that fd in to the
 * room (aka vector) with other existing clients */
void Server::handleNewConnection() {
    struct sockaddr_storage remoteaddr; // connector's address info
    int                     newfd;      // Newly accepte()ed socket fd
    socklen_t               addrlen;

    addrlen = sizeof(remoteaddr);

    if ((newfd = accept(_listener, (struct sockaddr*) &remoteaddr, &addrlen)) == -1)
        std::cerr << "accept" << std::endl;
    else {
        if (fcntl(newfd, F_SETFL, O_NONBLOCK) == -1) {
            std::cerr << "fcntl failed" << std::endl;
            close(newfd);
            return;
        }
        std::cout << "✅ New connection from " << getClientIP(remoteaddr) << " on socket " << newfd
                  << std::endl;

        addToTheRoom(newfd, &remoteaddr);
    }
}

void Server::handleClientData(int client_fd) {
    char buf[256];
    int  nbytes = recv(client_fd, buf, sizeof(buf), 0);

    // 1. Check health of the recv()
    if (nbytes <= 0) {
        if (nbytes == 0) // Connection lost
            std::cout << "pollserver: socket " << client_fd << " hung up" << std::endl;
        else
            perror("recv");

        _clients_to_disconnect.push_back(client_fd);
        return;
    }

    // 3. PROCESS (Success)
    User* user = _users[client_fd];
    user->appendBuffer(std::string(buf, nbytes));

    while (user->hasCompleteMessage()) {
        std::string command = user->extractMessage();

        std::cout << "HANDING OFF TO PARSER: \"" << command << "\" from User " << user->getFd()
                  << std::endl;

        ParsedCommand parsed = Parser::parse(command);
        if (parsed.err) {
            if (parsed.err == ERR_UNKNOWNMODE) {
                reply(parsed.err, *user, parsed.args[1], "");
            } else {
                reply(parsed.err, *user, "", "");
            }
			std::cout << "[DEBUG] parsed found error: " << parsed.err << std::endl;
            continue;
        }

        // --- DEBUGGING PARSER OUTPUT ---
        std::cout << "DEBUG: Parsed Command Name: '" << parsed.command << "'" << std::endl;
        std::cout << "DEBUG: Parsed Command Args (" << parsed.args.size() << "):";
        for (size_t i = 0; i < parsed.args.size(); ++i) {
            std::cout << " '" << parsed.args[i] << "'";
        }
        std::cout << std::endl;
        // --- END DEBUGGING PARSER OUTPUT ---

        executeCommand(*user, parsed);
    }
}

void Server::processConnections() {
    for (size_t i = 0; i < _pfds.size(); ++i) {
        struct pollfd& current = _pfds[i];
        if (!(current.revents & (POLLIN | POLLHUP | POLLERR))) continue; // Check events

        /* If the _listener rings, its a new user, give him a new fd and push him to the room
         * with the other clients */
        if (current.fd == _listener) {
            handleNewConnection();
            continue;
        }

        /* If the connection is not coming through the main door aka _listener its an already
         * existing client trying to communicate, so deal with him */
        handleClientData(current.fd);
    }

    /* Process the disconnections AFTER the event loop */
    for (size_t i = 0; i < _clients_to_disconnect.size(); ++i) {
        removeFromTheRoom(_clients_to_disconnect[i]);
    }
    _clients_to_disconnect.clear();
}
