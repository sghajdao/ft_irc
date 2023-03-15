/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messagerror.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sghajdao <sghajdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 16:55:19 by ibenmain          #+#    #+#             */
/*   Updated: 2023/03/15 15:49:23 by sghajdao         ###   ########.fr       */
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
/******************************/
/*         User Error         */
/******************************/
#define ERR_NEEDMOREPARAMS " :Not enough parameters"
#define ERR_ALREADYREGISTERED " :You may not reregister"

#define ERROR " :There is more parameters"
#define ERR_REGISTERED " :You must registred entre command <PASS> <USER> <NICK>"


#endif