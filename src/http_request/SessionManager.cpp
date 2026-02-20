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
#include <cstdlib>

SessionManager::SessionManager(void) : _counter(0)
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));
}

SessionManager::~SessionManager(void) {}

std::string	SessionManager::createSession(void)

{
	std::ostringstream	oss;
	std::string			id;
	SessionData			data;


	oss << std::hex;
	oss << static_cast<unsigned long>(std::time(NULL));
	oss << static_cast<unsigned long>(++_counter);
	oss << static_cast<unsigned long>(std::rand());

	id = oss.str();
	while (id.size() < 32)
		id += "0";

	data.createdAt = std::time(NULL);
	data.lastAccess = data.createdAt;
	_sessions[id] = data;
	return (id);
}

bool	SessionManager::sessionExists(const std::string &sessionId) const

{
	return (_sessions.find(sessionId) != _sessions.end());
}

SessionData	SessionManager::getSessionData(const std::string &sessionId) const

{
	std::map<std::string, SessionData>::const_iterator it = _sessions.find(sessionId);
	if (it != _sessions.end())
		return (it->second);
	return (SessionData());
}

void	SessionManager::setSessionValue(const std::string &sessionId, const std::string &key, const std::string &value)
{
	std::map<std::string, SessionData>::iterator it = _sessions.find(sessionId);
	if (it != _sessions.end())
	{
		it->second.values[key] = value;
		it->second.lastAccess = std::time(NULL);
	}
}

std::string	SessionManager::getSessionValue(const std::string &sessionId, const std::string &key) const
{
	std::map<std::string, SessionData>::const_iterator it = _sessions.find(sessionId);
	if (it != _sessions.end())
	{
		std::map<std::string, std::string>::const_iterator vit = it->second.values.find(key);
		if (vit != it->second.values.end())
			return (vit->second);
	}
	return ("");
}
