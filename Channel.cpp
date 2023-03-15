#include "User.hpp"
#include "Channel.hpp"
#include "server.hpp"

Channel::Channel(const string& name): _name(name) {}

Channel::~Channel() { }

const string& Channel::getName(void) const {
    return _name;
}

void Channel::broadcast(Server *server, int ignoreFd) const {
    map<int, User *>::const_iterator it;
    const string msg = server->createReplyForm();

    for(it = _userList.begin(); it != _userList.end(); ++it) {
        if (it->first == ignoreFd) continue;

        it->second->addToReplyBuffer(server->createReplyForm());
    }
}

const vector<string> Channel::getUserList(void) const {
    vector<string> userList;

    for (map<int, User *>::const_iterator it = _userList.begin(); it != _userList.end(); ++it) {
        string nickname = "";
        nickname += it->second->getNickname();
        userList.push_back(nickname);
    }
    return userList;
}

void Channel::addUser(int clientFd, User *user) {
    _userList.insert(make_pair(clientFd, user));
}

int Channel::deleteUser(int clientFd) {
    map<int, User *>::iterator it;
    string clientSource;

    it = _userList.find(clientFd);
    if (it == _userList.end()) return _userList.size();
    
    clientSource = it->second->getSource();
    _userList.erase(clientFd);

    if (_userList.empty()) return 0;
    return _userList.size();
}

User* Channel::findUser(const int clientFd) {
    map<int, User *>::iterator it;

    it = _userList.find(clientFd);
    if (it == _userList.end()) return NULL;
    return it->second;
}

User* Channel::findUser(const string& nickname) {
    map<int, User *>::iterator it;

    for(it = _userList.begin(); it != _userList.end(); ++it) {
        User *user = it->second;

        if (user->getNickname() == nickname) return user;
    }
    return NULL;
}
