#include "server.hpp"
#include "User.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./server <port> <password>\n";
        exit(EXIT_FAILURE);
    }

    int port = stoi(std::string(argv[1]));
    Server ircServer(port, argv[2]);

    cout << "Server created" << endl;
    try {
        ircServer.run();    
    } catch(exception &e) {
        e.what();
        ircServer.shutDown("Error while running server");
    }

    return 0;
}
