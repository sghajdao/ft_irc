#include "server.hpp"
#include "User.hpp"
#include "messagerror.hpp"
#include "Channel.hpp"

vector<string>	ft_split_line(string str)
{
	vector<string> key;
	if (str.empty())
		return key;
	char *tmp = strtok(const_cast<char *>(str.c_str()), ",");
	while (tmp != NULL)
	{
		key.push_back(tmp);
		tmp = strtok(NULL, ",");
	}
	return(key);
}

void Server::cmdNotice(User *user, const struct kevent& event) {
    if (_params.size() < 2) {
		sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
		return;
	}

    const vector<string> targetList = split(getParams()[0], ',');
    for (vector<string>::const_iterator it = targetList.begin(); it != targetList.end(); ++it) {
        string targetName = *it;
    	Channel *targetChannel;

        targetChannel = findChannelByName(targetName);
        if (targetChannel == NULL) {
			sendMessage(user, event, ERR_NOSUCHCHNL, 401);
			continue;
		}
		targetChannel->broadcast(this, -1);
    }
}

void Server::cmdKick(User *user, const struct kevent& event){
	if (_params.size() < 2) {
		sendMessage(user, event, ERR_NEEDMOREPARAMS, 461);
		return;
	}

	if (_params.size() == 2) {

		Channel *targetChannel = findChannelByName(_params[0]);
		if (targetChannel == NULL) {
			sendMessage(user, event, ERR_NOSUCHCHNL, 401);
			return;
		}

		if (targetChannel->findUserByFd(user->getFd()) == NULL){
			sendMessage(user, event, ERR_NOTINCHNL, 401);
			return;
		}

		const vector<string> targetUsers = split(_params[1], ',');
		for (vector<string>::const_iterator it = targetUsers.begin(); it != targetUsers.end(); ++it) {
			
			User *targetUser = targetChannel->findUserByNick(*it);
			if (targetUser == NULL) {
				sendMessage(user, event, ERR_USERNOTINCHNL, 401);
				continue;
			}

			int flag = targetChannel->deleteUser(targetUser->getFd());
			if (flag == 0) {deleteChannel(targetChannel->getName());}
		}
	}
}

void Server::cmdPart(User *user, const struct kevent& event, vector<string> tab)
{
	vector<string> channel_leave;
	vector<string> reason;
	map<string, Channel *>::iterator it;

	if (tab.empty())
	{
		sendMessage(user, event, " :Not enough parameters", 461);
		return ;
	}
	channel_leave = ft_split_line(tab[0]);
	reason = ft_split_line(tab[1]);
	for (int i = 0 ; i < channel_leave.size(); i++)
	{
		it = _allChannel.find(channel_leave[i]);
		if (it != _allChannel.end())
		{
			if (it->second->findUserIfExist(user->getFd()) && it->second->findOperatorIfExist(user->getFd()))
			{
				it->second->deleteUser(user->getFd());
				it->second->deleteOperator(user->getFd());
				user->deleteChannel(channel_leave[i]);
				if (reason.size() == 0)
					sendMessage(user, event, " :user deleted from channel " + channel_leave[i], 912);
				else
					sendMessage(user, event, " :user deleted from channel " + channel_leave[i] + " for reason " + reason[0], 912);
			}
			else if (it->second->findUserIfExist(user->getFd()))
			{
				it->second->deleteUser(user->getFd());
				user->deleteChannel(channel_leave[i]);
				if (reason.size() == 0)
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
	key_channel = ft_split_line(channel[1]);
	x = key_channel.size();
	for(int i = 0; i < name_channel.size(); i++)
	{
		map<string, Channel *>::iterator it;
		it = _allChannel.find(name_channel[i]);
		if ((name_channel[i][0] != '#' && name_channel[i][0] != '&') || (name_channel[i].size() == 1 && (name_channel[i][0] == '#' || name_channel[i][0] == '&')))
			sendMessage(user, event, " :Bad Channel Mask", 461);
		else if (it != _allChannel.end())
		{
			if (it->second->findUserIfExist(user->getFd()))
				sendMessage(user, event, " deja exist ", 475);
			else if (it->second->getFindPass() == 1)
			{
				if (x != 0)
				{
					if (it->second->getPassword() == key_channel[i])
					{
						it->second->addUser(event.ident, user);
						user->addChannel(name_channel[i]);
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
				user->addChannel(name_channel[i]);
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
				user->addChannel(name_channel[i]);
				tmp = new User(*user);
				tmp->setNickname("@" + tmp->getNickname());
				channel->addOperators(event.ident, tmp);
				channel->setFindPass(false);
				_allChannel.insert(make_pair(name_channel[i], channel));
				sendMessage(user, event, " :You are reregister in the channel " + name_channel[i], 461);
			}
			else if(x != 0)
			{
				Channel *channel;
				User 	*tmp;
				channel = new Channel(name_channel[i].c_str(), key_channel[i]);
				channel->addUser(event.ident, user);
				user->addChannel(name_channel[i]);
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
		cout << "channel: " << it->first << "pass is :" << it->second->getFindPass() << endl;
		it->second->getAllUser();
		it->second->getOperator();
	}
	cout << "--------end----------\n";
}


int Server::INVITE(User *user, const struct kevent event, vector<string> &invite)
{
	if (invite.empty())
		return (sendMessage(user, event, ERR_NEEDMOREPARAMS, 461), 0);
	string nickname = invite[0];
	string channel = invite[1];
	std::map<string, User *>::iterator it;
	
	Channel *t = findChannelByName(channel);
	if (t == NULL)
		return (sendMessage_INVITE(nickname, event, ERR_NOSUCHCHANNEL, 403), 0);
	User *t0 = t->findUserByNick(nickname);
	if (t0 == NULL)
		return (sendMessage_INVITE(channel, event, ERR_NOSUCHNICK, 401), 0);
	bool l = t->findOperatorIfExist(user->getFd());
	if (l == false)
		return (sendMessage_INVITE(channel, event, ERR_CHANOPRIVSNEEDED, 482), 0);
	if (t0 != NULL)
			return (sendMessage_INVITE(channel, event, ERR_USERONCHANNEL, 443), 0);
	
	sendMessage_INVITE(nickname + " ", event, channel, 341);
	return (0);
}