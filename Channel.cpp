/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sghajdao <sghajdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 01:11:04 by ibenmain          #+#    #+#             */
/*   Updated: 2023/04/14 00:24:12 by sghajdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Channel.hpp"
#include "server.hpp"

Channel::Channel(void){}
Channel::Channel(const string& name){
    _name = name;
    _password = "";
    _nametopic = "";
    _findPass = 0;
    _topic = 0;
    _invit = 0;
}

Channel::Channel(string name, string password){
    _name = name;
    _password = password;
    _nametopic = "";
    _findPass = 0;
    _topic = 0;
    _invit = 0;
};

Channel::~Channel() { }

const string& Channel::getName(void) const {
    return _name;
}

void Channel::broadcast(User *user ,Server *server, string option, int flag) const {
    map<int, User *>::const_iterator it;
    const string msg = server->createReplyForm(user);

    for(it = _userList.begin(); it != _userList.end(); ++it) {
        // if (it->first == ignoreFd) continue;
        if (flag == 0)
            it->second->addToReplyBuffer(server->createReplyForm(user));
        else if (flag == 1)
            it->second->addToReplyBuffer(":"+user->getNickname()+"!"+user->getUsername()+"@"+user->getHost()+ " JOIN :" + user->getChannelList().back() + "\n");
        else if (flag == 2) {
            it->second->addToReplyBuffer(":"+user->getNickname()+"!"+user->getUsername()+"@"+user->getHost()+ " PART " + user->getChannelLeave() + (user->getReason().empty() ? "" : (" :" + user->getReason())) + "\n");
        }
        else if (flag == 3) {
            it->second->addToReplyBuffer(":" + (user->getUsername().empty() ? "" : (user->getNickname() + "!" + user->getUsername())) + "@" + user->getHost() + option + "\n");
        }
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

void    Channel::setOperator()
{
    std::map<int, User *>::iterator it;
    it = _operators.begin();
    for (; it != _operators.end(); it++)
        it->second->setNickname("@" + it->second->getNickname());   
}

void Channel::addUser(int clientFd, User *user) {
    _userList.insert(make_pair(clientFd, user));
}

void Channel::addOperators(int clientFd, User *user) {
    _operators.insert(make_pair(clientFd, user));
}

int Channel::deleteUser(int clientFd) {
    map<int, User *>::iterator it;

    it = _userList.find(clientFd);
    if (it == _userList.end()) return _userList.size();
    {
        _userList.erase(it);
    }
    if (_userList.empty()) return 0;
    return _userList.size();
}

void Channel::deleteOperator(int clientFd) {
    map<int, User *>::iterator it;

    it = _operators.find(clientFd);
    if (it != _operators.end())
    {
        _operators.erase(it);
    }
}

User* Channel::findUserByFd(const int clientFd) {
    map<int, User *>::iterator it;

    it = _userList.find(clientFd);
    if (it == _userList.end()) return NULL;
    return it->second;
}

User* Channel::findFirstUserbyNick(string nick) {
    map<int, User *>::iterator it;

    it = _userList.begin();
    for (; it != _userList.end(); it++)
    {
        if (it->second->getNickname() == nick)
            return (it->second);
    }
    if (it == _userList.end()) return NULL;
    return it->second;
}

User* Channel::findSecondUser(string nick) {
    map<int, User *>::iterator it;
    for (it = _userList.begin() ; it != _userList.end(); it++)
    {
        if (it->second->getNickname() != nick)
            return(it->second);
    }
    return (NULL);
}

bool Channel::findOperatorIfExistByNick(string nick) {
    map<int, User *>::iterator it;
    it = _operators.begin();
    for (; it != _operators.end(); it++)
    {
        if (it->second->getNickname() == nick)
            return (true);
    }
    return (false);
}

bool Channel::findUserIfExistByNick(string nick) {
    map<int, User *>::iterator it;
    it = _userList.begin();
    for (; it != _userList.end(); it++)
    {
        if (it->second->getNickname() == nick)
            return (true);
    }
    return (false);
}

int Channel::getFdOfUser(string nick) {
    map<int, User *>::iterator it;
    it = _userList.begin();
    for (; it != _userList.end(); it++)
    {
        if (it->second->getNickname() == nick)
            return (it->first);
    }
    return (false);
}

int Channel::getFdOfOperator(string nick) {
    map<int, User *>::iterator it;
    it = _operators.begin();
    for (; it != _operators.end(); it++)
    {
        if (it->second->getNickname() == "@" + nick)
            return (it->first);
    }
    return (false);
}

bool Channel::findOperatorIfExist(const int clientFd) {
    map<int, User *>::iterator it;

    it = _operators.find(clientFd);
    if (it != _operators.end())
        return (true);
    return (false);
}

bool Channel::findUserIfExistByFd(const int clientFd) {
    map<int, User *>::iterator it;

    it = _userList.find(clientFd);
    if (it != _userList.end())
        return (true);
    return (false);
}

User* Channel::findUserByNick(const string& nickname) {
    map<int, User *>::iterator it;

    for(it = _userList.begin(); it != _userList.end(); ++it) {
        User *user = it->second;

        if (user->getNickname() == nickname) return user;
    }
    return NULL;
}

vector<string>   Channel::getAllUser()
{
    vector<string> vect;
    map<int, User *>::iterator it;
    it = _userList.begin();
    for (; it != _userList.end(); it++)
    {
        if (isOperator(it->second))
            vect.push_back("@" + it->second->getNickname());
        else
            vect.push_back(it->second->getNickname());
    }
    return (vect);
}

const string    Channel::getUser(int fd)
{
    map<int, User *>::iterator it;
    it = _userList.find(fd);
    return(it->second->getNickname());
}

void    Channel::getInvite(void)
{
    for (size_t i = 0; i < _invite.size(); i++)
        cout << "Invite: " << _invite[i] << endl;
}

string    Channel::getSecondOperator(void)
{
     map<int, User *>::iterator it;

    it = _userList.begin();
    return(it->second->getNickname());
}

bool Channel::getFindPass() const
{
    return (_findPass);
}

void Channel::setFindPass(bool pass)
{
    _findPass = pass;
}

bool Channel::getTopic() const
{
    return (_topic);
}

void Channel::setTopic(bool topic)
{
    _topic = topic;
}

bool Channel::getFoundtopic() const
{
    return (_foundtopic);
}

void Channel::setFoundtopic(bool foundtopic)
{
    _foundtopic = foundtopic;
}

string Channel::getNametopic()
{
    return (_nametopic);
}

void Channel::setNametopic(string nametopic)
{
    _nametopic = nametopic;
}

bool Channel::getInvit() const
{
    return (_invit);
}

void Channel::setInvit(bool invit)
{
    _invit = invit;
}

string Channel::getPassword() const
{
    return (_password);
}

void Channel::editPassword(string passwd)
{
    _password = passwd;
}

string Channel::getPassword(void)
{
    return(_password);
}

void Channel::deletePassword()
{
    _password.clear();
}

bool Channel::isOperator(User *user){
    for (map<int, User *>::const_iterator it = _operators.begin(); it != _operators.end(); ++it){
        if (user->getNickname() == (*it).second->getNickname()){
            return true;
        }
    }
    return false;
}

int Channel::checkInvit(std::string nick)
{
    for (size_t i = 0; i < _invite.size(); i++)
    {
      if (_invite[i] == nick)
        return(1);
    }
    return (0);
}

void Channel::deleteInvite(std::string nickname)
{
  std::vector<std::string>::iterator it;

  it = find(_invite.begin(), _invite.end(), nickname);
    if (it != _invite.end())
        _invite.erase(it);
}