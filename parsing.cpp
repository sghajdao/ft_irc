#include "server.hpp"
#include "User.hpp"
#include "messagerror.hpp"
#include "Channel.hpp"
#include <iostream>

int		Server::checkUserExist(vector<string> tab, User* user, const struct kevent& event)
{
	map<int, User *>::iterator it = _allUser.begin();
	for (; it != _allUser.end(); it++)
	{
		if (tab[0].compare(it->second->getUsername()) == 0)
			return (1);
	}
	return (0);
}

std::string	stringTolower(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return(str);
}

void Server::findCmd(string cmd) {
	vector<string> splitedCmd = split(cmd, ' ');

	for (vector<string>::size_type i = 0; i < splitedCmd.size(); ++i) {
		if (_command.empty()) {
            _command = splitedCmd[i];
            continue;
        }
		_params.push_back(splitedCmd[i]);
	}
}

size_t Server::checkCmd(User *user) {
	const size_t	brPos = user->getCmdBuffer().find('\r', 0);
	const size_t	bnPos = user->getCmdBuffer().find('\n', 0);

	if (brPos == string::npos && bnPos == string::npos) return string::npos;
	if (bnPos == string::npos) return brPos;
	if (brPos == string::npos) return bnPos;
	return min(brPos, bnPos);
}

void Server::handleCmd(User *user, const struct kevent& event) {
	size_t pos;

	while ((pos = checkCmd(user)) != string::npos) {
		if (pos == 0) {
			user->setCmdBuffer(user->getCmdBuffer().substr(1));
			continue;
		}
        findCmd(user->getCmdBuffer().substr(0, pos));
        __parssingCommand(user, event);
        _command.clear();
		_params.clear();
	}
}

vector<string> Server::split(const string& str, const char delimeter) {
    vector<string> splited;
    size_t cursorPos = 0;
    size_t delimeterPos;

    while ((delimeterPos = str.find(delimeter, cursorPos)) != string::npos) {
        splited.push_back(str.substr(cursorPos, delimeterPos - cursorPos));
        while (str.at(delimeterPos) == delimeter) {
            if (++delimeterPos == str.length()) return splited;
        }
        cursorPos = delimeterPos;
    }
    splited.push_back(str.substr(cursorPos));
    return splited;
}

User* Server::findClientByNickname(const string& nickname) const {
	map<int, User*>::const_iterator it;
	for (it = _allUser.cbegin(); it != _allUser.end(); ++it) {
		if (it->second->getNickname() == nickname) return it->second;
	}
	return NULL;
}

Channel* Server::findChannelByName(const string& name) const {
	if (name[0] != '#') return NULL;
	
	map<string, Channel *>::const_iterator it;
	for (it = _allChannel.cbegin(); it != _allChannel.end(); ++it) {
		if (it->second->getName() == name) return it->second;
	}
	return NULL;
}

/* IN JOIN COMMAND */
Channel* Server::addChannel(const string& name) {
	if (_allChannel.size() >= 30) return NULL;
	
	Channel *ch;

	ch = new Channel(name);
	_allChannel.insert(make_pair(name, ch));
	cout << "channel added: " << name << '\n';
	return ch;
}

void Server::deleteChannel(const string& name) {
	map<string, Channel *>::iterator it = _allChannel.find(name);
	Channel *ch = it->second;

	if (it == _allChannel.end()) return ;
	
	cout << "Delete channel from server: " << name << '\n';
	_allChannel.erase(name);
	delete ch;
}

const string Server::createReplyForm(void) const {
    string reply;

    vector<string>::const_iterator it;
    for (it = _params.begin(); it != _params.end(); ++it) {
        reply += (*it);
        if (*it != ":" && (it + 1) != _params.end()) reply += ' ';
    }
    reply += "\r\n";
    return reply;
}

void Server::cmdPrivmsg(User *user, const struct kevent& event) {
    if (_params.size() < 2) {
		sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
		return;
	}

    const vector<string> targetList = split(getParams()[0], ',');
    for (vector<string>::const_iterator it = targetList.begin(); it != targetList.end(); ++it) {
        string targetName = *it;
            User *targetUser;

            targetUser = findClientByNickname(targetName);
            if (targetUser == NULL) {
				sendMessage(user, event, ERR_NOSUCHNICK, 401);
				continue;
			}
            targetUser->addToReplyBuffer(user->getSource() + getCommand() + targetUser->getNickname() + ":" + getParams()[1]);
    }
}




/*
	LOOP:
	step 1: ghada t9albi 3la user libghti t invitie l chennel wax kain f map dyal users ila makanx return => error
	step 2: ghada tchofi dak l user li ghadi invite wax howa operator olala (wax 3ando l7a9 invite)
	LOOP:
	step 3: ghadit tchofi channel wax kaina ola la ila kanet ghadi t invite dak user l channel makantch a return => error
	(aytzad f map dyal _userList)

*/

// int Server::INVITE(User *user, const struct kevent event, vector<string> &invite)
// {
// 	if (invite.empty())
// 		return (sendMessage(user, event, ERR_NEEDMOREPARAMS, 461), 0);
// 	string nickname = invite[0];
// 	string channel = invite[1];
// 	std::map<string, User *>::iterator it;
	
// 	Channel *t = findChannelByName(channel);
// 	if (t == NULL)
// 		return (sendMessage_INVITE(nickname, event, ERR_NOSUCHCHANNEL, 403), 0);
// 	User *t0 = t->findUserByNick(nickname);
// 	if (t0 == NULL)
// 		return (sendMessage_INVITE(channel, event, ERR_NOSUCHNICK, 401), 0);
// 	bool l = t->findOperatorIfExistByNick(nickname);
// 	if (l == false)
// 		return (sendMessage_INVITE(channel, event, ERR_CHANOPRIVSNEEDED, 482), 0);
// 	if (t0 != NULL)
// 		return (sendMessage_INVITE(channel, event, ERR_USERONCHANNEL, 443), 0);
// 	sendMessage_INVITE(nickname + " ", event, channel, 341);
// 	return (0);
// }
