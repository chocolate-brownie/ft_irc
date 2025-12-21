#include "../../includes/Server.hpp"

Server::Server(int port, std::string password)
    : _port(port), _password(password), _fdsize(5)
{
    _commandMap["KICK"] = &Server::cmdKick;
    _commandMap["INVITE"] = &Server::cmdInvite;
    _commandMap["TOPIC"] = &Server::cmdTopic;
    _commandMap["MODE"] = &Server::cmdMode;
    _commandMap["JOIN"] = &Server::cmdJoin;
    _commandMap["PRIVMSG"] = &Server::cmdPrivmsg;
    _commandMap["NICK"] = &Server::cmdNick;
    _commandMap["USER"] = &Server::cmdUser;
    _commandMap["PART"] = &Server::cmdPart;
}

Server::~Server() {}

void Server::start()
{
    // Set up and get a listening socket
    if ((_listener = getListenerSocket()) == -1)
    {
        std::cerr << "error gettings listening socket" << std::endl;
        exit(1);
    }

    // Set the listening socket to non-blocking (crucial before poll())
    fcntl(_listener, F_SETFL, O_NONBLOCK);
    // Add the listening socket to the waiting room
    _pfds.push_back(makePollFds(_listener, POLLIN));

    std::cout << "🚀 Server listening on port " << _port << "..." << std::endl;

    // Main loop
    for (;;)
    {
        int num_events = poll(_pfds.data(), _pfds.size(), -1);
        if (num_events == -1)
        {
            std::cerr << "poll" << std::endl;
            exit(1);
        }

        processConnections();
    }

    //   addr_size = sizeof(their_addr);
    //   new_fd = accept(_listener, (struct sockaddr *)&their_addr, &addr_size);
    //   if (new_fd == -1)
    //     std::cerr << "Error: accept failed: " << std::strerror(errno) <<
    //     std::endl;

    //   close(new_fd);

    // sendAndReceiveData();
    // disconnectTheSocket();
}

struct addrinfo* Server::getAddressInfo()
{
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string portStr = portToString(_port);

    if ((status = getaddrinfo(NULL, portStr.c_str(), &hints, &res)) == -1)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(1);
    }
    return res;
}

int Server::getListenerSocket()
{
    struct addrinfo* p;
    int fd;
    int yes = 1;
    struct addrinfo* servinfo = getAddressInfo();

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            std::cerr << "server: socket" << std::endl;
            continue;
        }

        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            std::cerr << "setsockopt" << std::endl;
            exit(1);
        }

        if (bind(fd, p->ai_addr, p->ai_addrlen) == -1)
        {
            std::cerr << "server: bind" << std::endl;
            close(fd);
            continue;
        }

        break; // Success!
    }

    freeaddrinfo(
        servinfo); // after binding the socket I am done with this struct

    if (p == NULL)
    {
        std::cerr << "server: failed to bind" << std::endl;
        exit(1);
    }

    if (listen(fd, BACKLOG) == -1)
    { // start listening
        std::cerr << "server: listen" << std::endl;
        exit(1);
    }

    return fd;
}
