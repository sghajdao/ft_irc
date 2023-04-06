#include "server.hpp"
#include "User.hpp"
#include <iostream>

int check_error(char *av)
{
    if (!av)
        return (1);
    int i = 0;
    int x = atoi(av);
    while (av[i])
    {
        if (av[i] > '9' || av[i] < '0')
            return (1);
        i++;
    }
    if (x > 65535 || x == -1)
        return (1);
    return (0);
}

int main(int argc, char **argv)
{
    if (check_error(argv[1]) || argc != 3) {
        cerr << "Usage: ./ircserver <port> <password>\n";
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    Server ircServer(port, argv[2]);

    std::cout << "Server created" << endl;
    try {
        ircServer.run();
    } catch(exception &e) {
        e.what();
        ircServer.shutDown("Error while running server");
    }
    
    return 0;
}
