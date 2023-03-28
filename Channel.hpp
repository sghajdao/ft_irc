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
    string _password;
    bool _findPass;
		map<int, User *> _userList;
    map<int, User *> members;
		map<int, User *> _operators;
		// map<int, User *> bans;

    public:
        Channel(void);
        Channel(string name, string password):_name(name), _password(password){};
        Channel(const Channel& channel);
        Channel& operator=(const Channel& channel);

        Channel(const string& name);
        ~Channel();
        bool getFindPass() const;
        void setFindPass(bool pass);
        const string& getName(void) const;
        const vector<string> getUserList(void) const;
        void    setUserList(const User *user);
        void    getAllUser(void);
        const string    getUser(int fd);
        void    getOperator(void);
        string getPassword() const;
        void addUser(int clientFd, User *user);
        void addOperators(int clientFd, User *user);
        int deleteUser(int clientFd);
        void deleteOperator(int clientFd);
        User* findUserByFd(const int clientFd);
        bool findUserIfExist(const int clientFd);
        bool findOperatorIfExist(const int clientFd);
        User* findUserByNick(const string& nickname);
        void broadcast(Server *server, int ignoreF) const;
};

#endif
