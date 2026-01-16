#include "../../includes/Server.hpp"
#include "../../includes/User.hpp"

std::string Server::portToString(int port) {
    std::stringstream ss;
    ss << port;
    return ss.str();
}

struct pollfd Server::makePollFds(int fd, short events) const {
    struct pollfd p;
    p.fd      = fd;
    p.events  = events;
    p.revents = 0;
    return p;
}

std::string Server::getClientIP(const struct sockaddr_storage& addr) const {
    char        ipStr[INET6_ADDRSTRLEN]; // Buffer big enough for IPv6
    const void* s;

    // Determine if it's IPv4 or IPv6 and get the specific address pointer
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*) &addr;
        s                        = &(ipv4->sin_addr);
    } else {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*) &addr;
        s                         = &(ipv6->sin6_addr);
    }

    // Convert binary IP to string
    // inet_ntop returns the pointer to 'ipStr' on success
    if (inet_ntop(addr.ss_family, s, ipStr, sizeof(ipStr)) == NULL) return "Unknown IP";

    return std::string(ipStr);
}
