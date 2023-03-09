#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>
# include <exception>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/errno.h>
# include <vector>

using namespace std;

class User;
class Server {
    private:
        int _sd;
        int _kq;
        int _port;
        string _password;
        map<int, User *> _allUser;
        vector<struct kevent> eventList;
        struct kevent _waitingEvents[8];

        Server(void);
        Server(const Server& server);
        Server& operator=(const Server& server);

        void updateEvents(int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);

        void createNewClientSocket(void);

    public:
        Server(int port, string password);
        void recvClientData(const struct kevent& event);
        void sendDataToClient(const struct kevent& event);
        void handleEvent(const struct kevent& event);
        void run(void);
        void shutDown(const string& msg);
};

#endif