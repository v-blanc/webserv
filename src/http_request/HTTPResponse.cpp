/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/03/06 19:00:57 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"
#include <sys/stat.h>
#include <sys/wait.h>

HTTPResponse::HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message, SessionManager &sessionManager): _serverConfig(serverConfig), _sessionManager(sessionManager), _body(""), _contentLength(0), _response(""), _redirectCount(0)
{
	std::string existingId = request.getCookie("session_id");
	if (!existingId.empty() && _sessionManager.sessionExists(existingId))
		_sessionId = existingId;
	else
		_sessionId = _sessionManager.createSession();

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
			return ;
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
			oss <<"HTTP/1.1 "<<this->_status + " "<<this->_message<<"\r\n"<<"Location: "<<this->_newLocation<<"\r\n"<<"Set-Cookie: session_id="<<this->_sessionId<<"; Path=/; Max-Age=3600\r\n"<<"Content-Length: " << this->_contentLength << "\r\n\r\n" << this->_body;
		else
			oss <<"HTTP/1.1 "<<this->_status + " "<<this->_message<<"\r\n"<<"Set-Cookie: session_id="<<this->_sessionId<<"; Path=/; Max-Age=3600\r\n"<<"Content-Length: " << this->_contentLength << "\r\n\r\n" << this->_body;
		this->_response = oss.str();
	}

}

HTTPResponse::~HTTPResponse(){}

void	HTTPResponse::handleBadRequest(const std::string &status, const std::string &message)
{
	if (message == "Page Not Found")
	{
		this->_body = getLocalFileContent(this->_serverConfig.getRoot() + "/error/404.html");
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
		{
			int count = std::count(requestPath.begin(), requestPath.end(), '/');
			std::size_t secondLast = requestPath.rfind('/', pos - 1);
			std::size_t pred = pos;
			while (count > 2)
			{
				if (this->_serverConfig.isValidLocationPath(requestPath.substr(secondLast, (pred - secondLast))))
					break;
				else
				{
					pred = secondLast;
					secondLast = requestPath.rfind('/', secondLast - 1);
					count--;
				}
			}
			return (requestPath.substr(secondLast, (pred - secondLast)));

		}
	}
	return (requestPath);
}


void	HTTPResponse::handlePostMethod(HTTPRequest &request)
{
	std::string path =  handleRequestPath(request.getPathWithoutQuery(), false);
	LocationConfig myLocation;
	if (this->_serverConfig.isValidLocationPath(path))
		myLocation = this->_serverConfig.getLocationConfigByPath(path);
	else if (this->_serverConfig.isValidLocationPath("/"))
		myLocation = this->_serverConfig.getLocationConfigByPath("/");
	else
		fillLocationWithServerRules(myLocation);
	if (!myLocation.getReturn().empty())
	{
		if (++this->_redirectCount > 10)
			throw (HTTPRequest::StatusException("508", "Loop Detected"));
		std::string	newPath = myLocation.getReturn();
		request.setPath(newPath);
		this->_newLocation = newPath;
		return (handlePostMethod(request));
	}
	if (this->ismethodNotAllowed(myLocation.getLimitExcept(), request.getMethod()))
		throw HTTPRequest::StatusException("405", "Method Not Allowed");
	std::string interpreter;
	if (request.resolveCgiInterpreter(myLocation.getCgiHandler(), interpreter))
	{
		CgiRequestInfo cgiInfo;
		cgiInfo.interpreter = interpreter;
		cgiInfo.method = request.getMethod();
		cgiInfo.queryString = request.getQueryString();
		cgiInfo.pathWithoutQuery = request.getPathWithoutQuery();
		cgiInfo.contentLength = request.getContentLength();
		cgiInfo.contentType = request.getContentType();
		cgiInfo.body = request.getBody();
		throw CgiRequiredException(cgiInfo);
	}
	if (!request.getFileName().empty())
	{
		std::string fileName;
		if (!myLocation.getUploadStore().empty())
			fileName = myLocation.getRoot() + "/" + myLocation.getUploadStore() + '/' + request.getFileName();
		else
			fileName = myLocation.getRoot() + "/upload_store/" + request.getFileName();
		std::ofstream file(fileName.c_str());
		if (!file.is_open())
			throw HTTPRequest::StatusException("404", "Page Not Found");
		file << request.getBody();
		file.close();
		this->_status = "201";
		this->_message = "Created";
		this->_body = getLocalFileContent(myLocation.getRoot() + "/upload.html");
	}
	if (this->_body.empty())
		this->_body = getLocalFileContent(myLocation.getRoot() + "/submit.html");
}

bool	HTTPResponse::ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod)
{
	std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), myMethod);
	if (it == methods.end())
		return (true);
	return (false);
}

void HTTPResponse::handleIndexFile(const LocationConfig &myLocation, std::string &path)
{
	std::vector<std::string> indices = myLocation.getIndex();
	int fd = -1;
	std::string rightPath;
	for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); ++it)
	{
		std::string	rightIndex = *it;
		rightPath = myLocation.getRoot();
		rightPath = rightPath.substr(1, rightPath.size());
		rightPath += path + rightIndex;
		fd = open(rightPath.c_str(), O_RDONLY);
		if (fd != -1)
			break;
	}
	if (fd == -1)
	{
		if (myLocation.getAutoindex())
		{
			try
			{
				if (path == "/")
					this->_body = generateAutoindexHTML("./www");
				else
				{
					path.erase(path.size() - 1);
					this->_body = generateAutoindexHTML("./www/" + path);
				}
			}
			catch (std::runtime_error &e)
			{
				throw (HTTPRequest::StatusException("404", "Page Not Found"));
			}
		}
		else
			throw (HTTPRequest::StatusException("404", "Page Not Found"));
	}
	else
	{
		close(fd);
		this->_body = getLocalFileContent(rightPath.c_str());
	}
}

void HTTPResponse::fillLocationWithServerRules(LocationConfig &location)
{
	location.setAutoindex(this->_serverConfig.getAutoindex());
	location.setClientMaxBodySize(this->_serverConfig.getClientMaxBodySize());
	location.setRoot(this->_serverConfig.getRoot());
	location.setIndex(this->_serverConfig.getIndex());
}

void HTTPResponse::handleRessource(HTTPRequest &request)
{
	std::string ressource =  handleRequestPath(request.getPathWithoutQuery(), true);
	if (!ressource.empty() && ressource[0] == '/')
		ressource.erase(0, 1);
	ressource = this->_serverConfig.getRoot() + request.getPathWithoutQuery();
	std::ifstream file(ressource.c_str());
	if (!file.is_open())
		throw HTTPRequest::StatusException("404", "Page Not Found");
	file.close();
	this->_body = getLocalFileContent(ressource);
}

void HTTPResponse::handleGetMethod(HTTPRequest &request)
{
	if (request.getPathWithoutQuery() == "/session")
	{
		std::string visits = this->_sessionManager.getSessionValue(this->_sessionId, "visits");
		int count = 0;
		if (!visits.empty())
		{
			std::istringstream iss(visits);
			iss >> count;
		}
		++count;
		std::ostringstream countStr;
		countStr << count;
		this->_sessionManager.setSessionValue(this->_sessionId, "visits", countStr.str());

		std::ostringstream html;
		html << "<!DOCTYPE html>\n"
			<< "<html lang=\"fr\">\n<head>\n"
			<< "<meta charset=\"UTF-8\">\n"
			<< "<title>Cookie Demo - Compteur de visites</title>\n"
			<< "<style>\n"
			<< "body { font-family: Arial, sans-serif; background: #1a1a2e; color: #e0e0e0; "
			<< "display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }\n"
			<< ".card { background: #16213e; border-radius: 16px; padding: 40px 60px; text-align: center; "
			<< "box-shadow: 0 8px 32px rgba(0,0,0,0.3); }\n"
			<< "h1 { color: #e94560; margin-bottom: 10px; }\n"
			<< ".count { font-size: 72px; font-weight: bold; color: #0f3460; "
			<< "background: #e94560; border-radius: 50%; width: 120px; height: 120px; "
			<< "display: flex; align-items: center; justify-content: center; margin: 20px auto; }\n"
			<< ".info { background: #0f3460; padding: 15px 20px; border-radius: 8px; margin-top: 20px; "
			<< "font-size: 14px; word-break: break-all; }\n"
			<< ".label { color: #a0a0a0; font-size: 14px; }\n"
			<< "p { margin: 8px 0; }\n"
			<< "</style>\n</head>\n<body>\n"
			<< "<div class=\"card\">\n"
			<< "<h1>Cookie Demo</h1>\n"
			<< "<p class=\"label\">Nombre de visites</p>\n"
			<< "<div class=\"count\">" << count << "</div>\n"
			<< "<p>Rechargez la page pour incrementer le compteur.</p>\n"
			<< "<div class=\"info\">\n"
			<< "<p class=\"label\">Session ID</p>\n"
			<< "<p>" << this->_sessionId << "</p>\n"
			<< "</div>\n"
			<< "</div>\n"
			<< "</body>\n</html>\n";

		this->_body = html.str();
		this->_status = "200";
		this->_message = "OK";
		return ;
	}
	std::string path =  handleRequestPath(request.getPathWithoutQuery(), false);
	LocationConfig	myLocation;
	if (this->_serverConfig.isValidLocationPath(path))
	{
		myLocation = this->_serverConfig.getLocationConfigByPath(path);
	}
	else if (this->_serverConfig.isValidLocationPath("/"))
		myLocation = this->_serverConfig.getLocationConfigByPath("/");
	else
		fillLocationWithServerRules(myLocation);
	if (!myLocation.getReturn().empty())
	{
		if (++this->_redirectCount > 10)
			throw (HTTPRequest::StatusException("508", "Loop Detected"));
		std::string	newPath = myLocation.getReturn();
		request.setPath(newPath);
		this->_newLocation = newPath;
		return (handleGetMethod(request));
	}
	if (this->ismethodNotAllowed(myLocation.getLimitExcept(), request.getMethod()))
		throw HTTPRequest::StatusException("405", "Method Not Allowed");

	std::string interpreter;
	if (request.resolveCgiInterpreter(myLocation.getCgiHandler(), interpreter))
	{
		CgiRequestInfo	cgiInfo;

		cgiInfo.interpreter = interpreter;
		cgiInfo.method = request.getMethod();
		cgiInfo.queryString = request.getQueryString();
		cgiInfo.pathWithoutQuery = request.getPathWithoutQuery();
		cgiInfo.contentLength = request.getContentLength();
		cgiInfo.contentType = request.getContentType();
		cgiInfo.body = request.getBody();
		throw (CgiRequiredException(cgiInfo));
	}
	std::string requestPath = request.getPathWithoutQuery();
	if (path == "/" || requestPath.at(requestPath.size() - 1) == '/')
		handleIndexFile(myLocation, requestPath);
	else
		handleRessource(request);
}

static void	delete_recursive(const char *filename);

void	HTTPResponse::handleDeleteMethod(HTTPRequest &request)

{
	std::string const		requestPath = request.getPathWithoutQuery();
	std::string const		locationPath = handleRequestPath(requestPath, false);
	struct stat				st;
	LocationConfig 			location;

	try
	{
		location = this->_serverConfig.getLocationConfigByPath(locationPath);
	}
	catch(const std::out_of_range& e)
	{
		location = this->_serverConfig.getLocationConfigByPath("/");
	}
	if (!this->_serverConfig.isValidLocationPath(locationPath))
	{
		location = this->_serverConfig.getLocationConfigByPath("/");
	}

	std::string const		filename = location.getRoot() + requestPath;
	const char*				filename_c_str = filename.c_str();
	if (!location.getReturn().empty())
	{
		if (++this->_redirectCount > 10)
			throw (HTTPRequest::StatusException("508", "Loop Detected"));
		std::string	newPath = location.getReturn();
		request.setPath(newPath);
		this->_newLocation = newPath;
		return (handleGetMethod(request));
	}
	if (this->ismethodNotAllowed(location.getLimitExcept(), request.getMethod()))
			throw HTTPRequest::StatusException("405", "Method Not Allowed");
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
	if (S_ISDIR(st.st_mode))
		delete_recursive(filename_c_str);
	else
		std::remove(filename_c_str);
}

static void	delete_recursive(const char *base_path)

{
	struct stat		st;
	DIR*			dir;
	struct dirent	*entry;

	if (stat(base_path, &st) < 0)
		return ;
	if (!S_ISDIR(st.st_mode))
		std::remove(base_path);
	dir = opendir(base_path);
	if (!dir)
		return ;
	while ((entry = readdir(dir)) != NULL)
	{
		if (!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
			continue ;
		std::string	child_path = base_path;
		if (child_path.empty() || child_path[child_path.size() - 1] != '/')
			child_path += '/';
		child_path += entry->d_name;
		delete_recursive(child_path.c_str());
	}
	closedir(dir);
	std::remove(base_path);
}
