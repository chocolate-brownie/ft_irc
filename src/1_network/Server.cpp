#include "../../includes/Server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password), _fdsize(5) {
	time_t now = time(0);
    char *dt = ctime(&now); // ctime returns a string ending with \n
    std::string timeStr(dt);
    // Remove the newline that ctime adds at the end
    if (!timeStr.empty() && timeStr[timeStr.length()-1] == '\n') {
        timeStr.erase(timeStr.length()-1);
    }
    
    this->_startTime = timeStr;
}

Server::~Server() {}

void Server::start() {
    // Set up and get a listening socket
    if ((_listener = getListenerSocket()) == -1) {
        std::cerr << "error gettings listening socket" << std::endl;
        exit(1);
    }

    // Set the listening socket to non-blocking (crucial before poll())
    fcntl(_listener, F_SETFL, O_NONBLOCK);
    // Add the listening socket to the waiting room
    _pfds.push_back(makePollFds(_listener, POLLIN));

    std::cout << "🚀 Server listening on port " << _port << "..." << std::endl;

    // Main loop
    for (;;) {
        int num_events = poll(_pfds.data(), _pfds.size(), -1);
        if (num_events == -1) {
            std::cerr << "poll" << std::endl;
            exit(1);
        }

        processConnections();
    }

    // disconnectTheSocket();
}

struct addrinfo* Server::getAddressInfo() {
    struct addrinfo hints, *res;
    int             status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    std::string portStr = portToString(_port);

    if ((status = getaddrinfo(NULL, portStr.c_str(), &hints, &res)) == -1) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(1);
    }
    return res;
}

int Server::getListenerSocket() {
    struct addrinfo* p;
    int              fd;
    int              yes      = 1;
    struct addrinfo* servinfo = getAddressInfo();

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "server: socket" << std::endl;
            continue;
        }

        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            std::cerr << "setsockopt" << std::endl;
            exit(1);
        }

        if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
            std::cerr << "server: bind" << std::endl;
            close(fd);
            continue;
        }

        break; // Success!
    }

    freeaddrinfo(servinfo); // after binding the socket I am done with this struct

    if (p == NULL) {
        std::cerr << "server: failed to bind" << std::endl;
        exit(1);
    }

    if (listen(fd, BACKLOG) == -1) { // start listening
        std::cerr << "server: listen" << std::endl;
        exit(1);
    }

    return fd;
}


std::string Server::getStartTime() const
{
	return (_startTime);
}