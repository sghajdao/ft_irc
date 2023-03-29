#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <map>
# include <set>
# include <vector>

# include "server.hpp"

using namespace std;

class User;

class Channel {
    public:
		string _name;
		string _nametopic;
    string _password;
    bool _findPass;
    bool _topic;
    bool _foundtopic;
    bool _invit;
		map<int, User *> _userList;
    map<int, User *> members;
		map<int, User *> _operators;
		// map<int, User *> bans;

    public:
        Channel(void);
        Channel(string name, string password);
        Channel(const Channel& channel);
        Channel& operator=(const Channel& channel);

        Channel(const string& name);
        ~Channel();
        bool getFindPass() const;
        void setFindPass(bool pass);
        bool getTopic() const;
        void setTopic(bool topic);
        bool getFoundtopic() const;
        void setNametopic(string nametopic);
        string getNametopic();
        void setFoundtopic(bool foundtopic);
        bool getInvit() const;
        void setInvit(bool pass);
        void deletePassword();
        void editPassword(string passwd);
        string getPassword(void);
        const string& getName(void) const;
        const vector<string> getUserList(void) const;
        void    setUserList(const User *user);
        void    getAllUser(void);
        const string    getUser(int fd);
        void    getOperator(void);
        map<int, User *>    addSecondOperators(void);
        string    getSecondOperator(void);
        string getPassword() const;
        void addUser(int clientFd, User *user);
        void addOperators(int clientFd, User *user);
        int deleteUser(int clientFd);
        void deleteOperator(int clientFd);
        User* findUserByFd(const int clientFd);
        bool findUserIfExistByFd(const int clientFd);
        bool findUserIfExistByNick(string nick);
        bool findOperatorIfExistByNick(string nick);
        bool findOperatorIfExist(const int clientFd);
        User* findUserByNick(const string& nickname);
        void broadcast(Server *server, int ignoreF) const;
        int getFdOfUser(string nick);
        int getFdOfOperator(string nick);
};

#endif
