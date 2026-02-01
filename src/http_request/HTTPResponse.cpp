/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/02/01 19:56:11 by yassinefahf      ###   ########.fr       */
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
				prepareResponse();
			}
			catch(const HTTPRequest::StatusException &e)
			{
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

void	HTTPResponse::prepareResponse()
{
	std::string response;

	if (this->_body != "")
		this->_contentLength = this->_body.size();
	std::ostringstream oss;
	oss <<"HTTP/1.1 200 OK\r\nContent-Length: " << this->_contentLength << "\r\n\r\n" << this->_body;
	this->_response = oss.str();
	// std::cout<<"response: "this->_response<<std::endl;
}

HTTPResponse::~HTTPResponse(){}

void	HTTPResponse::handleBadRequest(const std::string &status, const std::string &message)
{
	if (message == "Page not found")
	{
		this->_body = getLocalFileContent("www/error/404.html");
		this->_contentLength = this->_body.size();
	}
	std::ostringstream oss;
	oss <<"HTTP/1.1 "<<status<<" "<<message<<"\r\n";
	if (!this->_body.empty())
		oss<<"Content Length: "<<this->_contentLength<<"\r\n\r\n"<<this->_body;
	else
		oss<<"Content Length: 0\r\n\r\n";
	this->_response = oss.str();
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

void HTTPResponse::handleIndexFile(const LocationConfig &myLocation)
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
		// if (myLocation.getAutoindex()) TODO: gerer autoindex
		throw (HTTPRequest::StatusException("404", "Page Not Found"));
	}
	else
	{
		close(fd);
		this->_body = getLocalFileContent(rightPath.c_str());
	}
}

void HTTPResponse::handleGetMethod(const HTTPRequest &request)
{
	if (this->_serverConfig.isValidLocationPath(request.getPathWithoutQuery()))
	{
		LocationConfig	myLocation = this->_serverConfig.getLocationConfigByPath(request.getPathWithoutQuery());
		std::cout<<"my location: "<<myLocation.getPath()<<std::endl;
		if (this->ismethodNotAllowed(myLocation.getLimitExcept(), request.getMethod()))
			throw HTTPRequest::StatusException("405", "Method Not Allowed");// TODO: check autoindex rules, if path is file or folder
		std::string requestPath = request.getPathWithoutQuery();
		if (request.getPathWithoutQuery() == "/")
			handleIndexFile(myLocation);
		else if (request.getPathWithoutQuery().at(request.getPathWithoutQuery().size() - 1) == '/')
			this->_response = generateAutoindexHTML(request.getPathWithoutQuery());
		
	}
	else
		throw HTTPRequest::StatusException("404", "Page not found");
}
