/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenmain <ibenmain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 01:10:10 by ibenmain          #+#    #+#             */
/*   Updated: 2023/04/13 16:40:43 by ibenmain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "User.hpp"
#include "messagerror.hpp"
#include "Channel.hpp"
#include <iostream>

int		Server::checkUserExist(vector<string> tab, User* user, const struct kevent& event)
{
	(void)user;
	(void)event;
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
	
	for (unsigned long i = 0; i < cmd.length(); ++i){
		if (cmd[i] == ' ' || cmd[i] == '\t') {cmd[i] = ',';}
	}
	vector<string> splitedCmd = split(cmd, ',');

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

    while ((delimeterPos = str.find(delimeter, cursorPos)) != string::npos) { // TODO nops c++20
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

const string Server::createReplyForm(User *user) const {
	string msg;
	for (vector<string>::const_iterator it = _params.begin() + 1; it != _params.end(); ++it) {
		msg.append(*it + " ");
	}
	msg.append("\r\n");
	string prefix = ":" + user->getNickname() + (user->getUsername().empty() ? "" : "!" + user->getUsername()) + (user->ft_hostname().empty() ? "" : "@" + user->ft_hostname()) + " NOTICE " + user->getNickname() + ((_params.size() > 2 && msg.find(":") != 2) ? " :" : " ");
	prefix.append(msg);
	return prefix;
}

void Server::cmdPrivmsg(User *user, const struct kevent& event) {
    if (_params.size() < 2) {
		sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461);
		return;
	}

    const vector<string> targetList = split(getParams()[0], ',');
    for (vector<string>::const_iterator it = targetList.begin(); it != targetList.end(); ++it) {
        string targetName = *it;
            User *targetUser;

            targetUser = findClientByNickname(targetName);
            if (targetUser == NULL) {
				sendMessage_error(user->getNickname(), event, ERR_NOSUCHNICK, 401);
				continue;
			}
			string msg;
			for (vector<string>::const_iterator it = _params.begin() + 1; it != _params.end(); ++it) {
				if (it + 1 == _params.end()){
					msg.append(*it);
					continue;
				}
				msg.append(*it + " ");
			}
			msg.append("\r\n");
			if (msg[0] == ':') {msg.erase(msg.begin());}
			string prefix = ":" + user->getNickname() + (user->getNickname().empty() ? "" : "!~" + user->getNickname()) + (user->getHost().empty() ? "" : "@" + user->getHost()) + " PRIVMSG " + targetUser->getNickname() + (_params.size() > 2 ? " :" : " ");
			targetUser->addToReplyBuffer(prefix + msg);
    }
}

int calculate_size(vector<string> str)
{
    int length = 0;

   	for (size_t i = 0; i < str.size(); i++)
		length++;
    return length;

}

void Server::cmdJoin(User *user, const struct kevent& event, vector<string> channel)
{
	std::vector<string> name_channel;
	std::vector<string> key_channel;
	string str;
	int x = 0;
	if (channel.empty())
		return(sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461));
	name_channel = split(channel[0], ',');
	if (calculate_size(channel) > 1)
	{
		key_channel = split(channel[1], ',');
		x = calculate_size(key_channel);
	}
	for(size_t i = 0; i < name_channel.size(); i++)
	{
		map<string, Channel *>::iterator it;
		if ((name_channel[i][0] != '#' && name_channel[i][0] != '&') || (name_channel[i].size() == 1 && (name_channel[i][0] == '#' || name_channel[i][0] == '&')))
			return(sendMessage_error(name_channel[i], event, ERR_BADCHANMASK, 915));
		it = _allChannel.find(name_channel[i]);
		if (it != _allChannel.end())
		{
			if (it->second->findUserIfExistByFd(user->getFd()))
				return(sendMessage_error(user->getNickname(), event, " User already exists ", 911));
			if (it->second->getFindPass() == 1)
			{
				if (x != 0)
				{
					if (it->second->getInvit() == 1)
					{
						if (it->second->checkInvit(user->getNickname()))
						{
							if (it->second->getPassword() == key_channel[i])
							{
								it->second->addUser(event.ident, user);
								user->addChannelUser(name_channel[i]);
								it->second->broadcast(user, this, -1, 1);
								str = "353 " + user->getUsername() + " = " + it->second->getName() + " :";
								vector<string> vect;
								vect = it->second->getAllUser();
								for (size_t i = 0; i != vect.size(); i++)
								{
									if (i == 0)
										str += vect[i];
									else
										str += " " + vect[i];
								}
								str += "\n366 " + user->getUsername() + " " + it->second->getName() + " :End of NAMES list.";
								user->addToReplyBuffer(str + "\n");
							}
							else
								sendMessage_error(key_channel[i], event, ERR_PASSWDMISMATCH, 464);
						}
						else
							return(sendMessage_error(user->getNickname(), event, " :User not invited", 475));
					}
					else
					{
						if (it->second->getPassword() == key_channel[i])
						{
							it->second->addUser(event.ident, user);
							user->addChannelUser(name_channel[i]);
							it->second->broadcast(user, this, -1, 1);
							str = "353 " + user->getUsername() + " = " + it->second->getName() + " :";
							vector<string> vect;
							vect = it->second->getAllUser();
							for (size_t i = 0; i != vect.size(); i++)
							{
								if (i == 0)
									str += vect[i];
								else
									str += " " + vect[i];
							}
							str += "\n366 " + user->getUsername() + " " + it->second->getName() + " :End of NAMES list.";
							user->addToReplyBuffer(str + "\n");
						}
						else
							sendMessage_error(key_channel[i], event, ERR_PASSWDMISMATCH, 464);
					}
					
				}
				else
					sendMessage_error(user->getNickname(), event, ERR_BADCHANNELKEY, 475);
			}
			else
			{
				if (it->second->getInvit() == 1)
				{
					if (it->second->checkInvit(user->getNickname()))
					{
						it->second->addUser(event.ident, user);
						user->addChannelUser(name_channel[i]);
						it->second->broadcast(user, this, -1, 1);
						vector<string> vect;
						str = "353 " + user->getUsername() + " = " + it->second->getName() + " :";
						vect = it->second->getAllUser();
						for (size_t i = 0; i != vect.size(); i++)
						{
							if (i == 0)
								str += vect[i];
							else
								str += " " + vect[i];
						}
						str += "\n366 " + user->getUsername() + " " + it->second->getName() + " :End of NAMES list.";
						user->addToReplyBuffer(str + "\n");
					}
					else
						return(sendMessage_error(user->getNickname(), event, " :User not invited", 475));
				}
				else
				{
					it->second->addUser(event.ident, user);
					user->addChannelUser(name_channel[i]);
					it->second->broadcast(user, this, -1, 1);
					vector<string> vect;
					str = "353 " + user->getUsername() + " = " + it->second->getName() + " :";
					vect = it->second->getAllUser();
					for (size_t i = 0; i != vect.size(); i++)
					{
						if (i == 0)
							str += vect[i];
						else
							str += " " + vect[i];
					}
					str += "\n366 " + user->getUsername() + " " + it->second->getName() + " :End of NAMES list.";
					user->addToReplyBuffer(str + "\n");
				}

			}
		}
		else
		{
			if (x == 0)
			{
				Channel *channel;

				channel = new Channel(name_channel[i].c_str());
				str = "353 " + user->getUsername() + " = " + name_channel[i] + " :";
				channel->addUser(event.ident, user);
				user->addChannelUser(name_channel[i]);
				user->addChannelOperator(name_channel[i]);
				channel->addOperators(event.ident, user);
				channel->setFoundtopic(false);
				channel->setFindPass(false);
				channel->setInvit(false);
				_allChannel.insert(make_pair(name_channel[i], channel));
				channel->broadcast(user, this, -1, 1);
				vector<string> vect;
				vect = channel->getAllUser();
				for (size_t i = 0; i != vect.size(); i++)
				{
					if (i == 0)
						str += vect[i];
					else
						str += " " + vect[i];
				}
				str += "\n366 " + user->getUsername() + " " + name_channel[i] + " :End of NAMES list.";
				user->addToReplyBuffer(str + "\n");
			}
			else
			{
				Channel *channel;

				channel = new Channel(name_channel[i].c_str(), key_channel[i]);
				str = "353 " + user->getUsername() + " = " + name_channel[i] + " :";
				channel->addUser(event.ident, user);
				user->addChannelUser(name_channel[i]);
				channel->addOperators(event.ident, user);
				channel->setFoundtopic(false);
				channel->setFindPass(true);
				channel->setInvit(false);
				_allChannel.insert(make_pair(name_channel[i], channel));
				channel->broadcast(user, this, -1, 1);
				vector<string> vect;
				vect = channel->getAllUser();
				for (size_t i = 0; i != vect.size(); i++)
				{
					if (i == 0)
						str += vect[i];
					else
						str += " " + vect[i];
				}
				str += "\n366 " + user->getUsername() + " " + name_channel[i] + " :End of NAMES list.";
				user->addToReplyBuffer(str + "\n");
				x = x - 1;
			}
		}
	}
}

void Server::cmdPart(User *user, const struct kevent& event, vector<string> tab)
{
	vector<string> channel_leave;
	vector<string> reason;
	map<string, Channel *>::iterator it;
	int x = 0;
	if (tab.empty())
		return(sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461));
	channel_leave = split(tab[0], ',');
	if (calculate_size(tab) > 1)
	{
		reason = split(tab[1], ',');
		x = 1;
	}
	for (size_t i = 0 ; i < channel_leave.size(); i++)
	{
		it = _allChannel.find(channel_leave[i]);
		if (it != _allChannel.end())
		{
			if (it->second->findUserIfExistByFd(user->getFd()) && it->second->findOperatorIfExist(user->getFd()))
			{
				User 	*tmp;
				it->second->deleteUser(user->getFd());
				it->second->deleteOperator(user->getFd());
				user->deleteChannelUser(channel_leave[i]);
				if (it->second->_userList.size() != 0 && !it->second->findOperatorIfExist(user->getFd()))
				{
					tmp = it->second->findSecondUser(user->getNickname());
					it->second->addOperators(tmp->getFd(), tmp);
				}
				if (reason.size() == 0)
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " PART : " + channel_leave[i] + "\n");
				else
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " PART : " + channel_leave[i] + " for reason " + reason[0] + "\n");
			}
			else if (it->second->findUserIfExistByFd(user->getFd()))
			{
				it->second->deleteUser(user->getFd());
				user->deleteChannelUser(channel_leave[i]);
				if (x == 0)
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " PART : " + channel_leave[i] + "\n");
				else
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " PART : " + channel_leave[i] + " for reason " + reason[0] + "\n");
			}
			else
				sendMessage_error(user->getNickname(), event, " :You're not reregister in this channel :" + channel_leave[i], 915);
			if (it->second->_userList.empty() && it->second->_operators.empty())
			{
				delete it->second;
				_allChannel.erase(it);
			}
		}
		else
			sendMessage_error(channel_leave[i], event, ERR_NOSUCHCHANNEL, 403);
	}
}

void Server::cmdMode(User *user, const struct kevent& event, vector<string> tab)
{
	map<string, Channel *>::iterator it;
	string str;
	User *tmp;

	if (tab.size() < 2)
		return;
	cout << tab.size() << endl;
	if (tab.empty() || tab.size() > 3)
		return(sendMessage_error(user->getNickname(), event, " :More parameters", 461));
	it = _allChannel.find(tab[0]);
	cout << tab[0] << endl;
	if (it != _allChannel.end())
	{
		if (tab[1] == "+k")
		{
			if (it->second->findOperatorIfExist(user->getFd()) && it->second->getFindPass() == 1)
			{
				it->second->editPassword(tab[2]);
				sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " EDIT A PASSWORD : " + tab[0] + "\n");
			}
			else if(it->second->findOperatorIfExist(user->getFd()) && it->second->getFindPass() == 0)
			{
				it->second->editPassword(tab[2]);
				it->second->setFindPass(true);
				sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " ADD A PASSWORD : " + tab[0] + "\n");
			}
			else
				sendMessage_error(user->getNickname(), event, " You're not channel operator :", 916);
		}
		else if (tab[1] == "-k")
		{
			if (it->second->findOperatorIfExist(user->getFd()))
			{
				it->second->deletePassword();
				it->second->setFindPass(false);
				sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " DELETE A PASSWORD : " + tab[0] + "\n");
			}
			else
				sendMessage_error(user->getNickname(), event, " :Not a operator in this channel", 912);
		}
		else if(tab[1] == "+o")
		{
			if (it->second->findUserIfExistByFd(user->getFd()) && it->second->findOperatorIfExist(user->getFd()))
			{
				if (it->second->findOperatorIfExistByNick(tab[2]))
					return (sendMessage_error(user->getNickname(), event, " :You are operator in this channel", 912));
				else if (it->second->findUserIfExistByNick(tab[2]))
				{
					it->second->addOperators(it->second->getFdOfUser(tab[2]), it->second->findFirstUserbyNick(tab[2]));
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " ADD A OPERATOR : " + tab[0] + "\n");
				}
				else
					sendMessage_error(tab[2], event, " :You are not a member in this channel", 912);
			}
			else
				sendMessage_error(user->getNickname(), event, " :You're not channel operator ", 912);
		}
		else if(tab[1] == "-o")
		{
			if (it->second->findUserIfExistByFd(user->getFd()) && it->second->findOperatorIfExist(user->getFd()))
			{
				if (it->second->_operators.size() == 1 && it->second->_userList.size() == 1)
					return(sendMessage_error(user->getNickname(), event, " :You can't delete all operator", 912));
				else if (it->second->findOperatorIfExistByNick(tab[2]))
				{
					it->second->deleteOperator(it->second->getFdOfUser(tab[2]));
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " DELETE A OPERATOR : " + tab[0] + "\n");
				}
				if (it->second->_operators.size() == 0)
				{
					tmp = it->second->findSecondUser(tab[2]);
					it->second->addOperators(tmp->getFd(), tmp);
				}
			}
			else
				sendMessage_error(user->getNickname(), event, " :You're not channel operator ", 916);
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
		sendMessage_error(tab[0], event, ERR_NOSUCHCHANNEL, 403);
}

void Server::cmdTopic(User *user, const struct kevent& event, vector<string> tab)
{
	map<string, Channel *>::iterator it;

	if (tab.empty())
		return (sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461));
	if (tab.size() != 1 && tab.size() != 2)
		return (sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461));
	it = _allChannel.find(tab[0]);
	if (it != _allChannel.end())
	{
		if (it->second->findUserIfExistByNick(user->getNickname()) &&  it->second->getTopic() == 0)
		{
			if (it->second->getFoundtopic() == 0)
			{
				if (tab.size() == 1)
					sendMessage_error(tab[0], event, " :No topic in this channel ", 912);
				else
				{
					if (tab.size() == 2)
					{
						it->second->setNametopic(tab[1]);
						it->second->setFoundtopic(true);
					}
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " TOPIC : " + it->second->getNametopic() + "\n");
				}
			}
			else if (it->second->getFoundtopic() == 1)
			{
				if (tab.size() == 2)
					it->second->setNametopic(tab[1]);
				sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " TOPIC : " + it->second->getNametopic() + "\n");
			}
		}
		else if (it->second->findOperatorIfExistByNick(user->getNickname()) && it->second->getTopic() == 1)
		{
			if (it->second->getFoundtopic() == 0)
			{
				if (tab.size() == 1)
					sendMessage_error(tab[0], event, " :No topic in this channel ", 912);
				else
				{
					if (tab.size() == 2)
					{
						it->second->setNametopic(tab[1]);
						it->second->setFoundtopic(true);
					}
					sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " TOPIC : " + it->second->getNametopic() + "\n");
				}
			}
			else if (it->second->getFoundtopic() == 1)
			{
				if (tab.size() == 2)
					it->second->setNametopic(tab[1]);
				sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " TOPIC : " + it->second->getNametopic() + "\n");
			}
		}
		else if (it->second->findUserIfExistByNick(user->getNickname()) && it->second->getTopic() == 1)
		{
			if (tab.size() == 1 && it->second->getFoundtopic() == 1)
				sendMessage(event, ":"+user->getNickname()+"!~"+user->getUsername()+"@"+user->ft_hostname()+ " TOPIC : " + it->second->getNametopic() + "\n");
			else if (tab.size() == 1 && it->second->getFoundtopic() == 0)
				sendMessage_error(tab[0], event, " :No topic in this channel ", 912);
			else
				sendMessage_error(tab[0], event, " :Not a operator in this channel ", 912);
		}
		else
			return (sendMessage_error(tab[0], event, " :Not a mumber in this channel", 403));
	}
	else
		return (sendMessage_error(tab[0], event, ERR_NOSUCHCHANNEL, 403));
}

void Server::cmdQuit(User *user, const struct kevent& event, vector<string> tab)
{
	(void)event;
	(void)tab;

	User 	*tmp;
	vector<string> channel = user->getUser();
	map<string, Channel *>::iterator it;
	for (size_t i = 0; i < channel.size(); i++)
	{
		it = _allChannel.find(channel[i]);
		if (it != _allChannel.end())
		{
			if (it->second->findUserIfExistByFd(user->getFd()))
			{				
				it->second->deleteUser(user->getFd());
				it->second->deleteOperator(user->getFd());
			}
			if (it->second->_userList.size() != 0 && !it->second->findOperatorIfExist(user->getFd()))
			{
				tmp = it->second->findSecondUser(user->getNickname());
				it->second->addOperators(tmp->getFd(), tmp);
			}
			if (it->second->_userList.empty() && it->second->_operators.empty())
			{
				delete it->second;
				_allChannel.erase(channel[i]);
			}
		}
	}
	map<int, User *>::const_iterator it1;
	it1 = _allUser.find(user->getFd());
	if (it1 != _allUser.end())
	{
		delete it1->second;
		_allUser.erase(it1);
	}
	// close(user->getFd());
}
