/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenmain <ibenmain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 16:56:00 by ibenmain          #+#    #+#             */
/*   Updated: 2023/03/28 22:51:05 by ibenmain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <cstring>
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
class Channel;
class Server {
    private:
        int _sd;
        int _kq;
        int _port;
        string _password;
        string _command;
        vector<string> _params;
        map<int, User *> _allUser;
        map<string, Channel *> _allChannel;
        vector<struct kevent> eventList; // kernel event notification mechanism
        struct kevent _waitingEvents[8];

    public:
        Server(void);
        Server(const Server& server);
        const vector<string>& getParams(void) const;
        const string& getCommand(void) const;
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
        void __parssingCommand(User* user, const struct kevent& event);
        void checkPassword(std::vector<string> tab, User* user, const struct kevent& event);
        void sendMessage(User *user, const struct kevent& event, std::string msg, int code);
        void checkUser(std::vector<string> tab, User* user, const struct kevent& event);
        void checkNick(std::vector<string> tab, User* user, const struct kevent& event);
        void authentication(std::vector<string> tab, User* user, const struct kevent& event);
        void sendMessageWelcom(string buffer, User* user, const struct kevent& event);
        int  checkUserExist(std::vector<string> tab, User* user, const struct kevent& event);
        int	 checkNickExist(vector<string> tab, User* user, const struct kevent& event);
        void handleCmd(User *user, const struct kevent& event);
        size_t checkCmd(User *user);
        void findCmd(string cmd);
        vector<string> split(const string& str, const char delimeter);
        User* findClientByNickname(const string& nickname) const;
        Channel* findChannelByName(const string& name) const;
        Channel* addChannel(const string& name);
        void deleteChannel(const string& name);
        const string createReplyForm(void) const;
        void cmdPrivmsg(User *user, const struct kevent& event);
        void cmdJoin(User *user, const struct kevent& event, vector<string> channel);
        void cmdPart(User *user, const struct kevent& event, std::vector<string> tab);
        void cmdMode(User *user, const struct kevent& event, vector<string> tab);
        void cmdKick(User *user, const struct kevent& event, vector<string> tab);
        void creatChannel(string name_channel, User *user, const struct kevent& event, string key_channel);
};

#endif