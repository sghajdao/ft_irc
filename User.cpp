#include <unistd.h>
#include "User.hpp"

User::User(int fd, const string& host) : _sd(fd), _host(host), _isQuiting(false) { }

User::~User() {
    close(_sd);
}

int User::getFd(void) const {
    return _sd;
}

const string& User::getHost(void) const {
    return _host;
}

const string& User::getPassword(void) const {
    return _password;
}

const string User::getNickname(void) const {
    if (_nickname.empty()) return "*";
    
    return _nickname;
}

const string User::getSource(void) const {
    string source = getNickname();
    if (source.empty()) source = "*";
    source = source + "@" + _host;

    return source;
}

const string& User::getUsername(void) const {
    return _username;
}

const string& User::getCmdBuffer(void) const {
    return _cmdBuffer;
}

const string& User::getReplyBuffer(void) const {
    return _replyBuffer;
}

bool User::getIsQuiting(void) const {
    return _isQuiting;
}

bool User::getIsUser(void) const {
    return _user;
}
bool User::getIsPass(void) const {
    return _pass;
}

void User::setPassword(const string& pwd) {
    _password = pwd;
}

void User::setNickname(const string& nickname) {
    _nickname = nickname;
}

void User::setUsername(const string& username) {
    _username = username;
}

void User::setCmdBuffer(const string& str) {
    _cmdBuffer = str;
}

void User::clearCmdBuffer(void) {
    _cmdBuffer.clear();
}

void User::setReplyBuffer(const string& str) {
    _replyBuffer = str;
}

void User::clearReplyBuffer(void) {
    _replyBuffer.clear();
}

void User::addToCmdBuffer(const string& str) {
    _cmdBuffer.append(str);
}

void User::addToReplyBuffer(const string& str) {
    _replyBuffer.append(str);
}

void User::setIsQuiting(void) {
    _isQuiting = true;
}

void User::setIsUser(void) {
    _user = true;
}

void User::setIsPass(void) {
    _pass = true;
}

int  User::getRegistred(void) const
{
    return (registred);
}

void User::setRegistred(void)
{
    registred += 1;
}
