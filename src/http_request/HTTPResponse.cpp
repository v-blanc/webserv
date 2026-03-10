/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/03/10 06:08:21 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message, SessionManager &sessionManager) : _serverConfig(serverConfig), _sessionManager(sessionManager), _body(""), _contentLength(0), _response(""), _redirectCount(0)
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
		try
		{
			if (request.getMethod() == "GET")
				handleGetMethod(request);
			else if (request.getMethod() == "POST")
				handlePostMethod(request);
			else if (request.getMethod() == "DELETE")
				handleDeleteMethod(request);

			prepareGoodResponse();
		}
		catch (const HTTPRequest::StatusException &e)
		{
			handleBadRequest(e.getStatus(), e.getMessage());
		}
	}
}

void HTTPResponse::prepareGoodResponse()
{
	if (this->_response.empty())
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
			oss << "HTTP/1.1 " << this->_status + " " << this->_message << "\r\n"
				<< "Location: " << this->_newLocation << "\r\n"
				<< "Set-Cookie: session_id=" << this->_sessionId << "; Path=/; Max-Age=3600\r\n"
				<< "Content-Length: " << this->_contentLength << "\r\n\r\n"
				<< this->_body;
		else
			oss << "HTTP/1.1 " << this->_status + " " << this->_message << "\r\n"
				<< "Set-Cookie: session_id=" << this->_sessionId << "; Path=/; Max-Age=3600\r\n"
				<< "Content-Length: " << this->_contentLength << "\r\n\r\n"
				<< this->_body;
		this->_response = oss.str();
	}
}

HTTPResponse::~HTTPResponse() {}

std::string HTTPResponse::resolveRoot(const std::string &root)

{
	if (!root.empty() && root[0] == '/')
		return (root.substr(1));
	return (root);
}

void HTTPResponse::handleBadRequest(const std::string &status, const std::string &message)
{
	if (message == "Page Not Found")
	{
		std::map<std::size_t, std::string> const errorPages = this->_serverConfig.getErrorPage();
		std::map<std::size_t, std::string>::const_iterator it = errorPages.find(404);
		if (it != errorPages.end())
			this->_body = getLocalFileContent(resolveRoot(this->_serverConfig.getRoot()) + it->second);
		this->_contentLength = this->_body.size();
	}

	std::ostringstream oss;
	oss << "HTTP/1.1 " << status << " " << message << "\r\n";
	oss << "Content-Length: " << this->_contentLength << "\r\n";
	oss << "Set-Cookie: session_id=" << this->_sessionId << "; Path=/; Max-Age=3600\r\n";
	oss << "\r\n";
	if (!this->_body.empty())
		oss << this->_body;
	this->_response = oss.str();
}

std::string HTTPResponse::handleRequestPath(std::string requestPath, bool isFileName)
{
	if (isFileName)
	{
		std::size_t pos = requestPath.find_last_of('/');
		if (pos != std::string::npos)
			return (requestPath.substr(pos));
		return (requestPath);
	}

	std::string path = requestPath;

	while (path.size() > 1 && path[path.size() - 1] == '/')
		path.erase(path.size() - 1);

	while (!path.empty())
	{
		if (this->_serverConfig.isValidLocationPath(path))
			return (path);

		std::size_t pos = path.find_last_of('/');
		if (pos == std::string::npos)
			break;
		if (pos == 0)
		{
			if (this->_serverConfig.isValidLocationPath("/"))
				return ("/");
			break;
		}
		path = path.substr(0, pos);
	}

	return (requestPath);
}

void HTTPResponse::handlePostMethod(HTTPRequest &request)
{
	std::string path = handleRequestPath(request.getPathWithoutQuery(), false);
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
			throw(HTTPRequest::StatusException("508", "Loop Detected"));
		std::string newPath = myLocation.getReturn();
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
		cgiInfo.root = resolveRoot(myLocation.getRoot());
		throw CgiRequiredException(cgiInfo);
	}

	if (!request.getFileName().empty())
	{
		if (myLocation.getUploadStore().empty())
			throw HTTPRequest::StatusException("500", "Internal Server Error");

		std::string destPath = resolvePath(3,
										   myLocation.getRoot().c_str(),
										   myLocation.getUploadStore().c_str(),
										   request.getFileName().c_str());

		std::ofstream file(destPath.c_str(), std::ios::binary);
		if (!file.is_open())
			throw HTTPRequest::StatusException("500", "Internal Server Error");

		file << request.getBody();
		file.close();

		this->_status = "201";
		this->_message = "Created";
		this->_body = "File uploaded successfully: " + request.getFileName();
		return;
	}

	this->_body = request.getBody();
}

bool HTTPResponse::ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod)
{
	if (methods.empty())
		return (false);
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
		std::string rightIndex = *it;
		rightPath = resolveRoot(myLocation.getRoot());
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
				this->_body = generateAutoindexHTML(resolveRoot(myLocation.getRoot()) + path);
			}
			catch (std::runtime_error &e)
			{
				throw(HTTPRequest::StatusException("404", "Page Not Found"));
			}
		}
		else
			throw(HTTPRequest::StatusException("404", "Page Not Found"));
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
	std::string ressource = handleRequestPath(request.getPathWithoutQuery(), true);
	if (!ressource.empty() && ressource[0] == '/')
		ressource.erase(0, 1);
	ressource = resolveRoot(this->_serverConfig.getRoot()) + request.getPathWithoutQuery();
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
		return;
	}
	std::string path = handleRequestPath(request.getPathWithoutQuery(), false);
	LocationConfig myLocation;
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
			throw(HTTPRequest::StatusException("508", "Loop Detected"));
		std::string newPath = myLocation.getReturn();
		request.setPath(newPath);
		this->_newLocation = newPath;
		return (handleGetMethod(request));
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
		cgiInfo.root = resolveRoot(myLocation.getRoot());
		throw(CgiRequiredException(cgiInfo));
	}
	std::string requestPath = request.getPathWithoutQuery();
	if (path == "/" || requestPath.at(requestPath.size() - 1) == '/')
		handleIndexFile(myLocation, requestPath);
	else
		handleRessource(request);
}

static void delete_recursive(const char *filename);

void HTTPResponse::handleDeleteMethod(HTTPRequest &request)
{
	std::string const requestPath = request.getPathWithoutQuery();

	std::string locationPath = handleRequestPath(requestPath, false);
	LocationConfig location;

	if (this->_serverConfig.isValidLocationPath(locationPath))
		location = this->_serverConfig.getLocationConfigByPath(locationPath);
	else if (this->_serverConfig.isValidLocationPath("/"))
		location = this->_serverConfig.getLocationConfigByPath("/");
	else
		fillLocationWithServerRules(location);

	if (!location.getReturn().empty())
	{
		if (++this->_redirectCount > 10)
			throw(HTTPRequest::StatusException("508", "Loop Detected"));
		std::string newPath = location.getReturn();
		request.setPath(newPath);
		this->_newLocation = newPath;
		return (handleDeleteMethod(request));
	}

	if (this->ismethodNotAllowed(location.getLimitExcept(), request.getMethod()))
		throw HTTPRequest::StatusException("405", "Method Not Allowed");

	std::string const filename = resolvePath(2,
											 location.getRoot().c_str(),
											 requestPath.c_str());
	const char *cstr = filename.c_str();

	struct stat st;
	if (stat(cstr, &st) < 0)
	{
		this->_status = "204";
		this->_message = "No Content";
		this->_body.clear();
		return;
	}

	bool writable = false;
	uid_t euid = geteuid();
	if (euid == 0)
		writable = (st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) != 0;
	else if (euid == st.st_uid)
		writable = (st.st_mode & S_IWUSR) != 0;
	else if (getegid() == st.st_gid)
		writable = (st.st_mode & S_IWGRP) != 0;
	else
		writable = (st.st_mode & S_IWOTH) != 0;

	if (!writable)
		throw HTTPRequest::StatusException("403", "Forbidden");

	if (S_ISDIR(st.st_mode))
		delete_recursive(cstr);
	else
		std::remove(cstr);

	this->_status = "204";
	this->_message = "No Content";
	this->_body.clear();
}

static void delete_recursive(const char *base_path)

{
	struct stat st;
	DIR *dir;
	struct dirent *entry;

	if (stat(base_path, &st) < 0)
		return;
	if (!S_ISDIR(st.st_mode))
		std::remove(base_path);
	dir = opendir(base_path);
	if (!dir)
		return;
	while ((entry = readdir(dir)) != NULL)
	{
		if (!std::strcmp(entry->d_name, ".") || !std::strcmp(entry->d_name, ".."))
			continue;
		std::string child_path = base_path;
		if (child_path.empty() || child_path[child_path.size() - 1] != '/')
			child_path += '/';
		child_path += entry->d_name;
		delete_recursive(child_path.c_str());
	}
	closedir(dir);
	std::remove(base_path);
}
