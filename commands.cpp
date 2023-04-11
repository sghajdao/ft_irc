/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenmain <ibenmain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 01:10:59 by ibenmain          #+#    #+#             */
/*   Updated: 2023/04/11 16:43:30 by ibenmain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "User.hpp"
#include "messagerror.hpp"
#include "Channel.hpp"

void Server::cmdNotice(User *user, const struct kevent& event) {
    if (_params.size() < 2) {
		sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461);
		return;
	}

    const vector<string> targetList = split(getParams()[0], ',');
    for (vector<string>::const_iterator it = targetList.begin(); it != targetList.end(); ++it) {
        string targetName = *it;
    	Channel *targetChannel;

        targetChannel = findChannelByName(targetName);
        if (targetChannel == NULL) {
			sendMessage_error(user->getNickname(), event, ERR_NOSUCHCHNL, 401);
			continue;
		}
		targetChannel->broadcast(user , this, -1);
    }
}

void Server::cmdKick(User *user, const struct kevent& event){
	if (_params.size() < 2) {
		sendMessage(event, ERR_NEEDMOREPARAMS);
		return;
	}

	if (_params.size() == 2) {

		if ((_params[0][0] != '#' && _params[0][0] != '&') || (_params[0].size() == 1 && (_params[0][0] == '#' || _params[0][0] == '&')))
			sendMessage(event, " :Bad Channel Mask");

		Channel *targetChannel = findChannelByName(_params[0]);
		if (targetChannel == NULL) {
			sendMessage(event, ERR_NOSUCHCHNL);
			return;
		}

		if (targetChannel->findUserByFd(user->getFd()) == NULL){
			sendMessage(event, ERR_NOTINCHNL);
			return;
		}

		if (targetChannel->isOperator(user) == false){
			sendMessage(event, ERR_NOTCHNLOPER);
			return;
		}

		const vector<string> targetUsers = split(_params[1], ',');
		for (vector<string>::const_iterator it = targetUsers.begin(); it != targetUsers.end(); ++it) {
			
			User *targetUser = targetChannel->findUserByNick(*it);
			if (targetUser == NULL) {
				sendMessage(event, ERR_USERNOTINCHNL);
				continue;
			}

			int flag = targetChannel->deleteUser(targetUser->getFd());
			if (flag == 0) {deleteChannel(targetChannel->getName());}
		}
	}
}

void	Server::sendMessage_error(string nickname, const struct kevent& event, std::string msg, int code)
{
	int sendBytes;
	std::string name = nickname;
	name = to_string(code) + " " + name + msg + "\n";
	sendBytes = send(event.ident, name.c_str(), name.size(), 0);
	if (sendBytes <= 0) {
		if (sendBytes == -1 && errno == EAGAIN) {
			errno = 0;
			return;
		}
		cerr << "client send error!" << endl;
		_allUser.erase(event.ident);
		cout << "client disconnected: " << event.ident << '\n';
	}
}

// int Server::cmdInvite(User *user, const struct kevent event, vector<string> &invite)
// {
// 	if (invite.empty())
// 		return (sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461), 0);
// 	string nickname = invite[0];
// 	string channel = invite[1];

// 	Channel *t = findChannelByName(channel);
// 	if (t == NULL)
// 		return (sendMessage_error(nickname, event, ERR_NOSUCHCHANNEL, 403), 0);
// 	User *t0 = t->findUserByNick(nickname);
// 	if (t0 == NULL)
// 		return (sendMessage_error(channel, event, ERR_NOSUCHNICK, 401), 0);
// 	bool l = t->findOperatorIfExist(user->getFd());
// 	if (l == false)
// 		return (sendMessage_error(channel, event, ERR_CHANOPRIVSNEEDED, 482), 0);
// 	if (t0 != NULL)
// 			return (sendMessage_error(channel, event, ERR_USERONCHANNEL, 443), 0);
// 	sendMessage_error(nickname + " ", event, channel, 341);
// 	return (0);
// }

void Server::cmdInvite(User *user, const struct kevent event, vector<string> invite)
{
	map<string, Channel *>::iterator it;
	string nickname = invite[0];
	string channel = invite[1];

	if (invite.size() != 2)
		return(sendMessage_error(user->getNickname(), event, ERR_NEEDMOREPARAMS, 461));
	it = _allChannel.find(channel);
	if (it != _allChannel.end())
	{
		if (it->second->getInvit() == 1 && it->second->findOperatorIfExist(user->getFd()))
		{
			if (getUesrNickname(nickname))
				it->second->_invite.push_back(nickname);
			else
				return(sendMessage_error(nickname, event, " :User not found", 401));
		}
		// else if (it->second->getInvit() == 0 && it->second->findUserIfExistByFd(user->getFd()))
		// {
		// 	if (getUesrNickname(nickname))
		// 		it->second->_invite.push_back(nickname);
		// 	else
		// 		return(sendMessage_error(nickname, event, " :User not found", 401));
		// }
		// else
		// 	return (sendMessage_error(nickname, event, " :User not member in the channel", 401));
	}
	else
		return (sendMessage_error(nickname, event, ERR_NOSUCHCHANNEL, 403));
	cout << "--------begin--------\n";
	map<string, Channel *>::iterator it1;
	it1 = _allChannel.begin();
	for (; it1 != _allChannel.end(); it1++)
	{
		cout << "channel: " << it1->first << " pass is :" << it1->second->getFindPass() << endl;
		it1->second->getAllUser();
		it1->second->getOperator();
		it1->second->getInvite();
	}
	cout << "--------end----------\n";
}
