/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sghajdao <sghajdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 05:46:54 by mlalouli          #+#    #+#             */
/*   Updated: 2023/04/12 22:17:11 by sghajdao         ###   ########.fr       */
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
# include <stdbool.h>
# include <vector>
#include <netdb.h>
#include <sstream>


using namespace std;

class User;
class Channel;
class Server {
    public:
        int _sd;
        int _kq;
        int _port;
        string _password;
        string _command;
        vector<string> _params;
        map<int, User *> _allUser;
        map<string, Channel *> _allChannel;
        vector<struct kevent> eventList;
        struct kevent _waitingEvents[8];

    public:
        Server(void);
        Server(const Server& server);
        const vector<string>& getParams(void) const;
        const string& getCommand(void) const;
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
        void sendMessage(const struct kevent& event, std::string msg);
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
        void cmdInvite(User *user, const struct kevent event, vector<string> invite);
        void sendMessage_error(string nickname, const struct kevent& event, std::string msg, int code);
        const string createReplyForm(User *user) const;
        void cmdPrivmsg(User *user, const struct kevent& event);
        void cmdJoin(User *user, const struct kevent& event, vector<string> channel);
        void cmdPart(User *user, const struct kevent& event, std::vector<string> tab);
        void cmdMode(User *user, const struct kevent& event, vector<string> tab);
        void cmdTopic(User *user, const struct kevent& event, vector<string> tab);
        void cmdQuit(User *user, const struct kevent& event, vector<string> tab);
        void cmdNotice(User *user, const struct kevent& event);
        void cmdKick(User *user, const struct kevent& event);
        bool getUesrNickname(string nickname);
        void boot(const struct kevent& event);
        void sendMessage_bot(string nickname, const struct kevent& event);
};

#endif