#include "../includes/Server.hpp"

void signalHandler(int signum) {
    (void)signum;
    std::cout << "\nInterrupt signal (" << signum << ") received.\n";
    Server::Signal = true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

	// Listens for Ctrl+C
    signal(SIGINT, signalHandler); 
    signal(SIGQUIT, signalHandler);

    int         port     = atoi(argv[1]);
    std::string password = argv[2];

    try {
        Server server(port, password);
        server.start();
    } catch (const std::exception& e) { std::cerr << e.what() << std::endl; }

    return 1;
}
