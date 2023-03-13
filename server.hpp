/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenmain <ibenmain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 16:56:00 by ibenmain          #+#    #+#             */
/*   Updated: 2023/03/13 21:55:06 by ibenmain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <ctime>
# include <map>
# include <exception>
#include <iostream>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/errno.h>
# include <stdbool.h>
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

    public:
        Server(void);
        Server(const Server& server);
        Server& operator=(const Server& server);
        void updateEvents(int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        std::string getpassword();
        void createNewClientSocket(void);
        Server(int port, string password);
        void recvClientData(const struct kevent& event);
        void sendDataToClient(const struct kevent& event);
        void handleEvent(const struct kevent& event);
        void run(void);
        void shutDown(const string& msg);
        void parssingCommand(User* user, const struct kevent& event);
        void checkPassword(std::vector<string> tab, User* user, const struct kevent& event);
        void sendMessage(User *user, const struct kevent& event, std::string msg, int code);
        void checkUser(std::vector<string> tab, User* user, const struct kevent& event);
        void checkNick(std::vector<string> tab, User* user, const struct kevent& event);
        void authentication(std::vector<string> tab, User* user, const struct kevent& event);
        void sendMessageWelcom(string buffer, User* user, const struct kevent& event);
        int  checkUserExist(std::vector<string> tab, User* user, const struct kevent& event);
        int	 checkNickExist(vector<string> tab, User* user, const struct kevent& event);

};

#endif