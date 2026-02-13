/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabokhar <yabokhar@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 17:35:07 by yabokhar          #+#    #+#             */
/*   Updated: 2026/02/13 17:51:03 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "SessionManager.hpp"
#include <sstream>

#include <iostream>
SessionManager::SessionManager(void)

{
	std::cout << "<<<<<<< Session created" << std::endl;
}

SessionManager::~SessionManager(void)

{
	std::cout << "<<<<<<< Session destroyed" << std::endl;
}

std::string	generateSessionId(void)

{
	std::ostringstream	oss;

	(void)oss;
	return ("feur");
}
