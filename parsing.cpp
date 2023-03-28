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
        if (targetName[0] == '*') {
            Channel *targetChannel = this->findChannelByName(targetName);

            if (targetChannel == NULL) {
				sendMessage(user, event, ERR_NOSUCHNICK, 401);
				continue;
			}
            targetChannel->broadcast(this, user->getFd());
        } else {
            User *targetUser;

            targetUser = findClientByNickname(targetName);
            if (targetUser == NULL) {
				sendMessage(user, event, ERR_NOSUCHNICK, 401);
				continue;
			}
            targetUser->addToReplyBuffer(user->getSource() + getCommand() + targetUser->getNickname() + ":" + getParams()[1]);
			// cout << user->getSource() + getCommand() + targetUser->getNickname() + ":" + getParams()[1] << endl;
        }
    }
}

vector<string>	ft_split_line(string str)
{
	vector<string> key;

	if (str.find(","))
	{
		char *tmp = strtok(const_cast<char *>(str.c_str()), ",");
		while (tmp != NULL)
		{
			key.push_back(tmp);
			tmp = strtok(NULL, ",");
		}
	}
	else
		key[0] = str;
	return(key);
}

int calculate_size(vector<string> str)
{
    int length = 0;

   	for (int i = 0; i < str.size(); i++)
		length++;
    return length;

}

void	Server::creatChannel(string name_channel, User *user, const struct kevent& event, string key_channel)
{

}

void Server::cmdJoin(User *user, const struct kevent& event, vector<string> channel)
{
	std::vector<string> name_channel;
	std::vector<string> key_channel;
	int x = 0;
	if (channel.empty())
	{
		sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
		return ;
	}
	name_channel = ft_split_line(channel[0]);
	if (calculate_size(channel) > 1)
	{
		key_channel = ft_split_line(channel[1]);
		x = calculate_size(key_channel);
	}
	for(int i = 0; i < name_channel.size(); i++)
	{
		map<string, Channel *>::iterator it;

		it = _allChannel.find(name_channel[i]);
		if ((name_channel[i][0] != '#' && name_channel[i][0] != '&') || (name_channel[i].size() == 1 && (name_channel[i][0] == '#' || name_channel[i][0] == '&')))
		{
			sendMessage(user, event, " :Bad Channel Mask", 461);
			return ;
		}
		if (it != _allChannel.end())
		{
			if (it->second->findUserIfExistByFd(user->getFd()))
			{
				sendMessage(user, event, " User deja exist ", 475);
				return ;
			}
			if (it->second->getFindPass() == 1)
			{
				if (x != 0)
				{
					if (it->second->getPassword() == key_channel[i])
					{
						it->second->addUser(event.ident, user);
						user->addChannelUser(name_channel[i]);
						sendMessage(user, event, " :You are reregister in the channel " + name_channel[i], 461);
					}
					else
						sendMessage(user, event, ERR_BADCHANNELKEY, 475);
				}
				else
					sendMessage(user, event, ERR_BADCHANNELKEY, 475);
			}
			else
			{
				it->second->addUser(event.ident, user);
				user->addChannelUser(name_channel[i]);
				sendMessage(user, event, " :You are reregister in the channel " + name_channel[i], 461);
			}
		}
		else
		{
			if (x == 0)
			{
				Channel *channel;
				User 	*tmp;

				channel = new Channel(name_channel[i].c_str(), "");
				channel->addUser(event.ident, user);
				user->addChannelUser(name_channel[i]);
				user->addChannelOperator(name_channel[i]);
				tmp = new User(*user);
				tmp->setNickname("@" + tmp->getNickname());
				channel->addOperators(event.ident, tmp);
				_allChannel.insert(make_pair(name_channel[i], channel));
				sendMessage(user, event, " :You are reregister in the channel " + name_channel[i], 461);
			}
			else
			{
				Channel *channel;
				User 	*tmp;

				channel = new Channel(name_channel[i].c_str(), key_channel[i]);
				channel->addUser(event.ident, user);
				user->addChannelUser(name_channel[i]);
				user->addChannelOperator(name_channel[i]);
				tmp = new User(*user);
				tmp->setNickname("@" + tmp->getNickname());
				channel->addOperators(event.ident, tmp);
				channel->setFindPass(true);
				_allChannel.insert(make_pair(name_channel[i], channel));
				sendMessage(user, event, " :You are reregister in the channel " + name_channel[i], 461);
				x = x - 1;
			}
		}
	}
	cout << "--------begin--------\n";
	map<string, Channel *>::iterator it;
	it = _allChannel.begin();
	for (; it != _allChannel.end(); it++)
	{
		cout << "channel: " << it->first << " pass is :" << it->second->getFindPass() << endl;
		it->second->getAllUser();
		it->second->getOperator();
	}
	cout << "--------end----------\n";
}

void Server::cmdPart(User *user, const struct kevent& event, vector<string> tab)
{
	vector<string> channel_leave;
	vector<string> reason;
	map<string, Channel *>::iterator it;
	int x = 0;
	if (tab.empty())
	{
		sendMessage(user, event, " :Not enough parameters", 461);
		return ;
	}
	channel_leave = ft_split_line(tab[0]);
	if (calculate_size(tab) > 1)
	{
		reason = ft_split_line(tab[1]);
		x = 1;
	}
	for (int i = 0 ; i < channel_leave.size(); i++)
	{
		it = _allChannel.find(channel_leave[i]);
		if (it != _allChannel.end())
		{
			if (it->second->findUserIfExistByFd(user->getFd()) && it->second->findOperatorIfExist(user->getFd()))
			{
				string str;
				User 	*tmp;
				it->second->deleteUser(user->getFd());
				it->second->deleteOperator(user->getFd());
				user->deleteChannelUser(channel_leave[i]);
				user->deleteChannelOperator(channel_leave[i]);
				if (it->second->_userList.size() != 0 && !it->second->findOperatorIfExist(user->getFd()))
				{
					str = it->second->getSecondOperator();
					tmp = new User(user->getFd() + 1, "");
					tmp->setNickname("@" + str);
					it->second->addOperators(user->getFd() + 1, tmp);
				}
				if (reason.empty())
					sendMessage(user, event, " :user deleted from channel " + channel_leave[i], 912);
				else
					sendMessage(user, event, " :user deleted from channel " + channel_leave[i] + " for reason " + reason[0], 912);
			}
			else if (it->second->findUserIfExistByFd(user->getFd()))
			{
				it->second->deleteUser(user->getFd());
				user->deleteChannelUser(channel_leave[i]);
				if (x == 0)
					sendMessage(user, event, " :user deleted from channel " + channel_leave[i], 912);
				else
					sendMessage(user, event, " :user deleted from channel " + channel_leave[i] + " for reason " + reason[0], 912);
			}
			else
				sendMessage(user, event, " :You're not on that channel", 442);
			if (it->second->_userList.empty()  && it->second->_operators.empty())
				_allChannel.erase(channel_leave[i]);
		}
		else
			sendMessage(user, event, " :No such channel", 403);
	}
	cout << "--------begin--------\n";
	map<string, Channel *>::iterator it1;
	it1 = _allChannel.begin();
	for (; it1 != _allChannel.end(); it1++)
	{
		cout << "channel: " << it1->first << " pass is :" << it1->second->getFindPass() << endl;
		it1->second->getAllUser();
		it1->second->getOperator();
	}
	cout << "--------end----------\n";
}

void Server::cmdMode(User *user, const struct kevent& event, vector<string> tab)
{
	map<string, Channel *>::iterator it;
	if (tab.empty())
	{
		sendMessage(user, event, " :Not enough parameters", 461);
		return ;
	}
	it = _allChannel.find(tab[0]);
	if (it != _allChannel.end())
	{
		if (tab[1] == "+k")
		{
			if (it->second->findOperatorIfExist(user->getFd()) && it->second->getFindPass() == 1)
			{
				it->second->editPassword(tab[2]);
				sendMessage(user, event, " :You are edit a password to this channel", 461);
			}
			else if(it->second->findOperatorIfExist(user->getFd()) && it->second->getFindPass() == 0)
			{
				it->second->editPassword(tab[2]);
				it->second->setFindPass(true);
				sendMessage(user, event, " :You are add a password to this channel", 461);
			}
			else
				sendMessage(user, event, " :You are not a operator in this channel", 461);
		}
		else if (tab[1] == "-k")
		{
			if (user->SearchChannelOperator("@" + user->getNickname()))
			{
				it->second->deletePassword();
				it->second->setFindPass(false);
				sendMessage(user, event, " :You are delete a password to this channel", 461);
			}
			else
				sendMessage(user, event, " :You are not a operator in this channel", 461);
		}
		else if(tab[1] == "+o")
		{
			if (it->second->findUserIfExistByNick(tab[2]) && !it->second->findOperatorIfExistByNick(tab[2]))
			{
				User *tmp;
				tmp = new User(it->second->getFdOfUser(tab[2]), "");
				tmp->setNickname("@" + tab[2]);
				it->second->addOperators(it->second->getFdOfUser(tab[2]), tmp);
				tmp->addChannelOperator(it->second->getName());
				sendMessage(user, event, " You are add a operator in this channel", 461);
			}
			else if (it->second->findOperatorIfExistByNick(tab[2]))
				sendMessage(user, event, " this user is operator in this channel", 461);
			else
					sendMessage(user, event, " This user not a member in this channel", 461);
		}
		else if(tab[1] == "-o")
		{
			if (it->second->findUserIfExistByNick(tab[2]) && it->second->findOperatorIfExistByNick(tab[2]))
				it->second->deleteOperator(it->second->getFdOfUser(tab[2]));
			else
				sendMessage(user, event, " You are not a operator in this channel", 461);
		}
		else if (tab[1] == "+t")
			it->second->setTopic(true);
		else if (tab[1] == "-t")
			it->second->setTopic(false);
		else if (tab[1] == "+i")
			it->second->setInvit(true);
		else if (tab[1] == "-i")
			it->second->setInvit(false);
	}
	else
		sendMessage(user, event, " This channel not found", 461);
	cout << "--------begin--------\n";
	map<string, Channel *>::iterator it1;
	it1 = _allChannel.begin();
	for (; it1 != _allChannel.end(); it1++)
	{
		cout << "channel: " << it1->first << " pass is :" << it1->second->getFindPass() << endl;
		it1->second->getAllUser();
		it1->second->getOperator();
	}
	cout << "--------end----------\n";
}
