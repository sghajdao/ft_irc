#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <map>
# include <set>
# include <vector>

# include "server.hpp"

using namespace std;

class User;

class Channel {
    private:
		string _name;
		map<int, User *> _userList;

        Channel(void);
        Channel(const Channel& channel);
        Channel& operator=(const Channel& channel);

    public:
        Channel(const string& name);
        ~Channel();

        const string& getName(void) const;
        const vector<string> getUserList(void) const;

        void addUser(int clientFd, User *user);
        int deleteUser(int clientFd);
        User* findUser(const int clientFd);
        User* findUser(const string& nickname);
        void broadcast(Server *server, int ignoreF) const;
};

#endif
