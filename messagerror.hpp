/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messagerror.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlalouli <mlalouli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 16:55:19 by ibenmain          #+#    #+#             */
/*   Updated: 2023/03/26 17:33:17 by mlalouli         ###   ########.fr       */
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
#define ERR_NOSUCHNICK ":No such Nickname"
#define ERR_NOSUCHCHANNEL " :No such channel"
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
#define ERR_USERONCHANNEL " :is already on channel"


#endif