/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabokhar <yabokhar@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 18:45:00 by yabokhar          #+#    #+#             */
/*   Updated: 2026/01/21 18:45:00 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "structServer.h"
#include "GlobalConfig.hpp"
#include "HTTPRequest.hpp"
#include "Server.hpp"
#include <string>
#include <vector>

void	unchunkBody(std::string &body);

bool	tryLaunchCgi(
			HTTPRequest const& httpRequest,
			GlobalConfig &globalConfig,
			std::vector<Server> const &servers,
			ClientContext *clientContext,
			int epfd
		);

void	handleCgiEvent(
			struct epoll_event &event,
			int epfd
		);

#endif