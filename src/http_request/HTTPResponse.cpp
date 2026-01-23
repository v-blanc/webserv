/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/01/23 11:28:45 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(const HTTPRequest &request, const std::string &status, ServerConfig &serverConfig): _serverConfig(serverConfig) 
{
	if (status != "")
		handleBadRequest(status);
	else
	{
		if (request.getMethod() == "GET")
		{
			try
			{
				handleGetMethod(request);
			}
			catch(const HTTPRequest::StatusException &e)
			{
				handleBadRequest(e.getStatus());
			}
		}
		else if (request.getMethod() == "POST")
		{
			try
			{
				handlePostMethod(request);
			}
			catch (const HTTPRequest::StatusException &e)
			{
				handleBadRequest(e.getStatus());
			}
		}
		else
			return ;// TODO: handle delete method
		
	}
}

void HTTPResponse::handleGetMethod(const HTTPRequest &request)
{
	std::string myPath = this->_serverConfig.getRoot() + request.getPathWithoutQuery();
	int fd = open(myPath, O_RDONLY);
	if (fd == -1)
		throw (HTTPRequest::StatusException("404", "Not Found"));
	
}
