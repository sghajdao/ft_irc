/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messagerror.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: mlalouli <mlalouli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 05:47:27 by mlalouli          #+#    #+#             */
/*   Updated: 2023/03/29 21:54:00 by mlalouli         ###   ########.fr       */
=======
/*   By: ibenmain <ibenmain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 16:55:19 by ibenmain          #+#    #+#             */
/*   Updated: 2023/03/28 23:41:34 by ibenmain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*   Updated: 2023/03/24 22:08:16 by sghajdao         ###   ########.fr       */
>>>>>>> 77b5e503256ab2e2a6e08c68eadc0f5eaa3857a1
/*                                                                            */
/* ************************************************************************** */



#ifndef MESSAGERROR_HPP
#define MESSAGERROR_HPP


/******************************/
/*       PassWord Error       */
/******************************/
#define ERR_NEEDMOREPARAMS " :Not enough parameters"
#define ERR_ALREADYREGISTERED " :You may not reregister"
#define ERR_PASSWDMISMATCH " :Password incorrect"
/******************************/
/*       NickName Error       */
/******************************/
#define ERR_ERRONEUSNICKNAME " :Erroneus nickname"
#define ERR_USERONCHANNEL  " :is already on channel"
#define ERR_NICKNAMEINUSE " :Nickname is already in use"
/******************************/
/*         User Error         */
/******************************/
#define ERR_NEEDMOREPARAMS " :Not enough parameters"
#define ERR_ALREADYREGISTERED " :You may not reregister"

#define ERROR " :There is more parameters"
#define ERR_REGISTERED " :You must registred entre command <PASS> <USER> <NICK>"
#define ERR_BADCHANNELKEY " :Cannot join channel (+k)"
#define ERR_CHANOPRIVSNEEDED " :You're not channel operator"
/******************************/
/*         INVITE Error         */
/******************************/
#define ERR_NOSUCHNICK ":No such Nickname"
#define ERR_NOSUCHCHANNEL " :No such channel"
#define ERR_USERONCHANNEL " :is already on channel"
#define ERR_USERONCHANNEL " :is already on channel"

#define ERR_NOSUCHCHNL " :No such channel"
#define ERR_NOTINCHNL " :You're not on that channel"
#define ERR_USERNOTINCHNL " :They aren't on that channel"

#endif