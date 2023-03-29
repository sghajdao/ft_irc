#include "server.hpp"
#include "User.hpp"
#include "messagerror.hpp"
#include "Channel.hpp"

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
