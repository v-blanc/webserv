/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/02/11 16:16:34 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"
#include <sys/stat.h>

HTTPResponse::HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message): _serverConfig(serverConfig), _body(""), _contentLength(0), _response("")
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
				prepareGoodResponse();
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
				prepareGoodResponse();
			}
			catch (const HTTPRequest::StatusException &e)
			{
				handleBadRequest(e.getStatus(), e.getMessage());
			}
		}
		else if (request.getMethod() == "DELETE")
		{
			try
			{
				handleDeleteMethod(request);
				prepareGoodResponse();
			}
			catch(const HTTPRequest::StatusException& e)
			{
				handleBadRequest(e.getStatus(), e.getMessage());
			}
			
		}
		else
			return ;// TODO: handle delete method
		
	}
}

void	HTTPResponse::prepareGoodResponse()
{
	if(this->_response.empty())
	{
		if (this->_body != "")
			this->_contentLength = this->_body.size();
		std::ostringstream oss;
		if (this->_status.empty() && this->_message.empty())
		{
			this->_status = "200";
			this->_message = "OK";
		}
		if (!this->_newLocation.empty())
			oss <<"HTTP/1.1 "<<this->_status + " "<<this->_message<<"\r\n"<<"Location: "<<this->_newLocation<<"\r\n"<<"Content-Length: " << this->_contentLength << "\r\n\r\n" << this->_body;
		else
			oss <<"HTTP/1.1 "<<this->_status + " "<<this->_message<<"\r\nContent-Length: " << this->_contentLength << "\r\n\r\n" << this->_body;
		this->_response = oss.str();
	}

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

std::string	HTTPResponse::handleRequestPath(std::string requestPath, bool isFileName)
{
	std::size_t pos = requestPath.find_last_of('/');
	if (pos == 0 && isFileName == false)
		return (requestPath);
	if (pos != std::string::npos)
	{
		if (isFileName)
			return (requestPath.substr(pos, requestPath.size()));
		else
			return (requestPath.substr(0, pos));
	}
	return (requestPath);
}

void	HTTPResponse::handlePostMethod(const HTTPRequest &request)
{
	std::string path =  handleRequestPath(request.getPathWithoutQuery(), false);
	if (this->_serverConfig.isValidLocationPath(path))
	{
		LocationConfig myLocation = this->_serverConfig.getLocationConfigByPath(path);
		std::string fileName;
		if (!myLocation.getUploadStore().empty())
			fileName = myLocation.getUploadStore() + handleRequestPath(request.getPathWithoutQuery(), true);
		else
			fileName = "www/upload_store/" + handleRequestPath(request.getPathWithoutQuery(), true);
		std::ofstream file(fileName.c_str());
		if (!file.is_open())
			throw HTTPRequest::StatusException("505", "Internal Server Error");
		file << request.getBody();
		file.close();
	}
	else
		throw HTTPRequest::StatusException("404", "Page Not Found");
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
	std::cout<<"ici"<<std::endl;
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
		if (myLocation.getAutoindex())
			this->_response = generateAutoindexHTML("./www");
		else
			throw (HTTPRequest::StatusException("404", "Page Not Found"));
	}
	else
	{
		close(fd);
		this->_body = getLocalFileContent(rightPath.c_str());
	}
}

void HTTPResponse::handleRessource(HTTPRequest &request)
{
	std::string ressource =  handleRequestPath(request.getPathWithoutQuery(), true);
	ressource = "www/" + ressource;
	std::ifstream file(ressource.c_str());
	if (!file.is_open())
		throw HTTPRequest::StatusException("404", "Page Not Found");
	file.close();
	this->_body = getLocalFileContent(ressource);
}

void HTTPResponse::handleGetMethod(HTTPRequest &request)
{
	std::string path =  handleRequestPath(request.getPathWithoutQuery(), false);
	if (this->_serverConfig.isValidLocationPath(path))
	{
		LocationConfig	myLocation = this->_serverConfig.getLocationConfigByPath(path);
		if (!myLocation.getReturn().empty())
		{
			std::string	newPath = myLocation.getReturn();
			request.setPath(newPath);
			this->_status = "301";
			this->_message = "Moved Permanently";
			this->_newLocation = newPath;
			return (handleGetMethod(request));
		}
		if (this->ismethodNotAllowed(myLocation.getLimitExcept(), request.getMethod()))
			throw HTTPRequest::StatusException("405", "Method Not Allowed");// TODO: check autoindex rules, if path is file or folder
		std::string requestPath = request.getPathWithoutQuery();
		if (path == "/")
			handleIndexFile(myLocation);
		else if (request.getPathWithoutQuery().at(request.getPathWithoutQuery().size() - 1) == '/')
		{
			if (myLocation.getAutoindex())
				this->_response = generateAutoindexHTML(request.getPathWithoutQuery());
			else
				throw HTTPRequest::StatusException("404", "Page not found");
		}
		else
			handleRessource(request);
	}
	else
		handleRessource(request);
}

void	HTTPResponse::handleDeleteMethod(const HTTPRequest &request)

{
	std::string const	path = handleRequestPath(request.getPathWithoutQuery(), false);
	std::string const	filename = "www" + path;
	const char*			filename_c_str = filename.c_str();
	struct stat			st;

	if (stat(filename_c_str, &st) < 0)
	{
		this->_status = "204";
		this->_message = "No Content";
		this->_body.clear();
		return ;
	}
	if (access(filename_c_str, W_OK) < 0)
	{
		this->_status = "403";
		this->_message = "Forbidden";
		this->_body = "Permission denied";
		return ;
	}
}
