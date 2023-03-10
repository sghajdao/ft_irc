/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sghajdao <sghajdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 16:56:18 by ibenmain          #+#    #+#             */
/*   Updated: 2023/03/13 16:13:17 by sghajdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "messagerror.hpp"
#include "server.hpp"
#include "User.hpp"

Server::Server(int port, string password): _sd(-1), _kq(-1), _port(port), _password(password) {
	struct sockaddr_in serverAddr;
	if ((_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		shutDown("socket() error");
	
	memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);
	if (fcntl(_sd, F_SETFL, O_NONBLOCK) < 0)
		shutDown("fcntl() error");
	updateEvents(_sd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (::bind(_sd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        shutDown("bind() error");

	if (listen(_sd, 5) == -1)
        shutDown("listen() error");
}

void Server::updateEvents(int socket, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent event;
	/* initialize a struct kevent object, which is used to represent an event */
	EV_SET(&event, socket, filter, flags, fflags, data, udata);
	eventList.push_back(event);
}

void Server::createNewClientSocket(void) {
	int clientSocket;
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	char hostStr[INET_ADDRSTRLEN];
	User *user;

	memset(hostStr, 0, sizeof(hostStr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	if ((clientSocket = accept(_sd, (struct sockaddr *)&clientAddr, &addrLen)) == -1) {
		cerr << "aceept() failed! Check errno : " << errno << endl;
		errno = 0;
		return ;
	}
	if (_allUser.size() >= 30) {
		cout << "Server reached max number of user" << endl;
		close(clientSocket);
		return ;
	}
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	inet_ntop(AF_INET, &clientAddr.sin_addr, hostStr, INET_ADDRSTRLEN);
	cout << "accept new client: " << clientSocket << " / Host : " << hostStr << endl;
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);

	updateEvents(clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	updateEvents(clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);

	user = new User(clientSocket, hostStr);
	_allUser.insert(make_pair(clientSocket, user));
}

void Server::run() {
	int numOfEvents;

	if ((_kq = kqueue()) == -1)
        throw(runtime_error("kqueue() error"));
	cout << "listening..." << endl;
	while (1) {
        numOfEvents = kevent(_kq, &eventList[0], eventList.size(), _waitingEvents, 8, NULL);
        if (numOfEvents == -1)
            shutDown("kevent() error");
        eventList.clear();
        for (int i = 0; i < numOfEvents; ++i)
            handleEvent(_waitingEvents[i]);
    }
}

void Server::recvClientData(const struct kevent& event) {
	char buf[501];
	map<int, User *>::iterator it = _allUser.find(event.ident);
	User* targetUser = it->second;
	int recvBytes;

	if (it == _allUser.end()) return ;
	recvBytes = recv(event.ident, buf, 500, 0);
	if (recvBytes <= 0) {
		if (recvBytes == -1 && errno == EAGAIN) {
			errno = 0;
			return;
		}
		cerr << "client recv error!" << endl;
		_allUser.erase(event.ident);
		cout << "client disconnected: " << event.ident << '\n';
	} else {
		buf[recvBytes] = '\0';
		targetUser->addToCmdBuffer(buf);
		handleCmd(targetUser, event);
		// Server::parssingCommand(targetUser, event);
	}
	targetUser->clearCmdBuffer();
}

void	Server::sendMessage(User *user, const struct kevent& event, std::string msg, int code)
{
	int sendBytes;
	std::string name = user->getNickname();
	name = ":" + name + " * " + to_string(code) + msg + "\n";
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

void Server::sendDataToClient(const struct kevent& event) {
	map<int, User *>::iterator it = _allUser.find(event.ident);
	User* targetUser = it->second;
	int sendBytes;

	if (it == _allUser.end()) return ;
	if (targetUser->getReplyBuffer().empty()) return;

	sendBytes = send(event.ident, targetUser->getReplyBuffer().c_str(), targetUser->getReplyBuffer().length(), 0);
	if (sendBytes == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			errno = 0;
			return ;
		}
		cerr << "client send error!" << endl; 
	} else {

		targetUser->setReplyBuffer(targetUser->getReplyBuffer().substr(sendBytes));
		if (targetUser->getIsQuiting() && targetUser->getReplyBuffer().empty()) _allUser.erase(event.ident);
	}
}

void Server::handleEvent(const struct kevent& event) {
	if (event.flags & EV_ERROR) {
		if (event.ident == (const uintptr_t)_sd)
			throw(runtime_error("server socket error"));
		else {
			User *targetUser = _allUser[event.ident];

			cerr << "client socket error" << endl;
		}
	} else if (event.filter == EVFILT_READ) {
		if (event.ident == (const uintptr_t)_sd)
			createNewClientSocket();
		else
			recvClientData(event);}
}

void Server::shutDown(const string& msg) {
	if (_sd != -1)
		close(_sd);
	if (_kq != -1)
		close(_kq);
	for (map<int, User *>::iterator it = _allUser.begin(); it != _allUser.end(); it++) {
		delete it->second;
	}
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

std::string Server::getpassword()
{
	return(_password);
}