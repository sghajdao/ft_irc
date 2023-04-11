/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibenmain <ibenmain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 01:11:01 by ibenmain          #+#    #+#             */
/*   Updated: 2023/04/11 16:42:11 by ibenmain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <map>
# include <set>
# include <vector>
# include "server.hpp"

using namespace std;

class User;

class Channel {
    public:
		std::string _name;
		std::string _nametopic;
    std::string _password;
    bool _findPass;
    bool _topic;
    bool _foundtopic;
    bool _invit;
		std::map<int, User *> _userList;
    // std::map<int, User *> members;
		std::map<int, User *> _operators;
    std::vector<std::string> _invite;
		// std::map<int, User *> bans;

    public:
        Channel(void)
        {
          
        };
        Channel(std::string name, std::string password);
        Channel(const Channel& channel);
        Channel& operator=(const Channel& channel);
        Channel(const std::string& name);
        ~Channel();
        bool getFindPass() const;
        void setFindPass(bool pass);
        bool getTopic() const;
        void setTopic(bool topic);
        bool getFoundtopic() const;
        void setNametopic(std::string nametopic);
        std::string getNametopic();
        void setFoundtopic(bool foundtopic);
        bool getInvit() const;
        void setInvit(bool pass);
        void deletePassword();
        void editPassword(std::string passwd);
        std::string getPassword(void);
        const std::string& getName(void) const;
        const std::vector<std::string> getUserList(void) const;
        void setUserList(const User *user);
        void getAllUser(void);
        const std::string    getUser(int fd);
        void    getOperator(void);
        void    getInvite(void);
        std::map<int, User *>    addSecondOperators(void);
        std::string getSecondOperator(void);
        std::string getPassword() const;
        void addUser(int clientFd, User *user);
        void addOperators(int clientFd, User *user);
        int deleteUser(int clientFd);
        void deleteOperator(int clientFd);
        User* findUserByFd(const int clientFd);
        User* findSecondUser(std::string nick);
        User* findFirstUser();
        bool findUserIfExistByFd(const int clientFd);
        bool findUserIfExistByNick(std::string nick);
        bool findOperatorIfExist(const int clientFd);
        bool findOperatorIfExistByNick(std::string nick);
        User* findUserByNick(const std::string& nickname);
        void broadcast(User *user ,Server *server, int ignoreFd) const ;
        int getFdOfUser(std::string nick);
        int getFdOfOperator(std::string nick);
        bool isOperator(User *user);
        int checkInvit(std::string nick)
        {
            for (size_t i = 0; i < _invite.size(); i++)
            {
              if (_invite[i] == nick)
                return(1);
            }
            return (0);
        };
        void deleteInvite(std::string nickname)
        {
          std::vector<std::string>::iterator it;

          it = _invite.begin();
          for (; it != _invite.end(); it++)
          {
            if (it->find(nickname))
              _invite.erase(it);
          }
        };
        User* findFirstUserbyNick(string nick);
        void setOperator();
};

#endif
