/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/01/30 15:08:55 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(const HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message): _serverConfig(serverConfig), _body(""), _contentLength(0), _response("")
{
	if (status != "")
		handleBadRequest(status, message);
	else
	{
		if (request.getMethod() == "GET")
		{
			try
			{
				handleGetMethod(request);
				prepareResponse(request);
			}
			catch(const HTTPRequest::StatusException &e)
			{
				std::cout<<e.what()<<std::endl;
				handleBadRequest(e.getStatus(), e.getMessage());
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
				handleBadRequest(e.getStatus(), e.getMessage());
			}
		}
		else
			return ;// TODO: handle delete method
		
	}
}

void	HTTPResponse::prepareResponse(const HTTPRequest &request)
{
	std::string response;

	(void)request;
	// if (this->_body != "")
		// this->_contentLength = this->_body;
	std::ostringstream oss;
	oss <<"HTTP/1.1 200 OK\r\nContent-Length: " << this->_contentLength << "\r\n\r\n" << this->_body;
	this->_response = oss.str();
	// std::cout<<"response: "this->_response<<std::endl;
}

HTTPResponse::~HTTPResponse(){}

void	HTTPResponse::handleBadRequest(const std::string &status, const std::string &message)
{
	(void)status;
	(void)message;
}

void	HTTPResponse::handlePostMethod(const HTTPRequest &request)
{
	if (this->_serverConfig.isValidLocationPath(request.getPathWithoutQuery()))
	{
		LocationConfig myLocation = this->_serverConfig.getLocationConfigByPath(request.getPathWithoutQuery());
		
	}
}

bool	HTTPResponse::ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod)
{
	std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), myMethod);
	if (it == methods.end())
		return (true);
	return (false);
}

void HTTPResponse::handleGetMethod(const HTTPRequest &request)
{
	if (this->_serverConfig.isValidLocationPath(request.getPathWithoutQuery()))
	{
		LocationConfig	myLocation = this->_serverConfig.getLocationConfigByPath(request.getPathWithoutQuery());
		// std::vector<std::string> methods = myLocation.getLimitExcept();
		if (this->ismethodNotAllowed(myLocation.getLimitExcept(), request.getMethod()))
			throw HTTPRequest::StatusException("405", "Method Not Allowed");// TODO: check autoindex rules, if path is file or folder
		if (request.getPathWithoutQuery() == "/")
		{
			std::vector<std::string> indices = myLocation.getIndex();
			int fd;
			std::string rightPath;
			for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); ++it)
			{
				std::string	rightIndex = *it;
				rightPath = myLocation.getRoot();
				rightPath = rightPath.substr(1, rightPath.size());
				rightPath += '/' + rightIndex;
				fd = open(rightPath.c_str(), O_RDONLY);
				if (fd != -1)
					break;
			}
			if (fd == -1)
			{
				throw (HTTPRequest::StatusException("404", "Not Found"));
			}
			else
			{
				close(fd);
				this->_body = getLocalFileContent(rightPath.c_str());
			}
		}
	}
}
