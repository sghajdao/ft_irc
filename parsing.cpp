#include "server.hpp"
#include "User.hpp"
#include "messagerror.hpp"

void	Server::checkPassword(std::vector<string> tab, User *user, const struct kevent& event)
{
	if (tab.size() < 2)
	 	sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
	else if(tab.size() > 2)
	 	sendMessage(user, event, ERROR, 404);
	else
	{
		if (tab[1].compare(getpassword()) == 0)
		{
	 		user->setPassword(tab[1]);
			user->setIsPass();
		}
		else
	 		sendMessage(user, event, ERR_PASSWDMISMATCH, 464);
	}
}

int		Server::checkUserExist(vector<string> tab, User* user, const struct kevent& event)
{
	map<int, User *>::iterator it = _allUser.begin();
	for (; it != _allUser.end(); it++)
	{
		if (tab[1].compare(it->second->getUsername()) == 0)
			return (1);
	}
	return (0);
}

int		Server::checkNickExist(vector<string> tab, User* user, const struct kevent& event)
{
	map<int, User *>::iterator it = _allUser.begin();
	for (; it != _allUser.end(); it++)
	{
		if (tab[1].compare(it->second->getNickname()) == 0)
			return (1);
	}
	return (0);
}

void	Server::checkUser(std::vector<string> tab, User* user, const struct kevent& event)
{
	if (tab.size() < 5)
	 	sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
	else if(tab.size() > 5)
	 	sendMessage(user, event, ERROR, 404);
	else
	{
		if (checkUserExist(tab, user, event))
			sendMessage(user, event, ERR_ALREADYREGISTERED, 464);
		else
		{
	 		user->setUsername(tab[1]);
			user->setIsUser();
		}
	}
}

void	Server::checkNick(std::vector<string> tab, User* user, const struct kevent& event)
{
	if (tab.size() < 2)
	 	sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
	else if(tab.size() > 2)
	 	sendMessage(user, event, ERROR, 404);
	else
	{
		if (checkNickExist(tab, user, event))
			sendMessage(user, event, ERR_ALREADYREGISTERED, 464);
		else
	 		user->setNickname(tab[1]);
	}
}


std::string	stringTolower(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return(str);
}

void	Server::parssingCommand(User* user, const struct kevent& event)
{
	/* string		cmd;
	vector<string> tab;
	string command = user->getCmdBuffer();
	command.erase(std::remove(command.begin(), command.end(), '\n'), command.cend());
	if (command.empty())
		return ;
	char *token = strtok((char *)command.c_str(), " ");
	while (token != NULL)
	{
		tab.push_back(token);
		token = strtok(NULL, " ");
	}
	cmd = stringTolower(tab[0]); */
	if (_command != "pass" && _command != "user" && _command != "nick")
		sendMessage(user, event, ERR_REGISTERED, 000);
	else if (_command == "pass" && _command == "user" && _command == "nick"){
		if (_command.compare("pass") == 0 && !user->getIsPass())
			Server::checkPassword(_params, user, event);
		else if(_command.compare("user") == 0 && !user->getIsUser())
			Server::checkUser(_params, user, event);
		else if(_command.compare("nick") == 0)
			Server::checkNick(_params, user, event);
	}
	user->clearCmdBuffer();
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
			continue;
		}
        findCmd(user->getCmdBuffer().substr(0, pos));
        parssingCommand(user, event);
		// EXECUTE HERE
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