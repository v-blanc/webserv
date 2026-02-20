/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabokhar <yabokhar@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 17:32:56 by yabokhar          #+#    #+#             */
/*   Updated: 2026/02/13 17:49:38 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <string>
#include <map>
#include <ctime>

struct SessionData

{
	std::map<std::string, std::string>		values;
	time_t									createdAt;
	time_t									lastAccess;
};

class SessionManager

{
	public:
		SessionManager(void);
		~SessionManager(void);

		std::string							createSession(void);
		bool								sessionExists(const std::string &sessionId) const;
		SessionData							getSessionData(const std::string &sessionId) const;
		void								setSessionValue(const std::string &sessionId, const std::string &key, const std::string &value);
		std::string							getSessionValue(const std::string &sessionId, const std::string &key) const;
	private:
		std::map<std::string, SessionData>	_sessions;
		unsigned long						_counter;
};

#endif
