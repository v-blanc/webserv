/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 14:04:09 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/02/12 13:14:26 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"
#include <sys/stat.h>
#include <sys/wait.h>

HTTPResponse::HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message, SessionManager &sessionManager): _serverConfig(serverConfig), _sessionManager(sessionManager), _body(""), _contentLength(0), _response("")
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

void	HTTPResponse::handlePostMethod(HTTPRequest &request)
{
	std::string path =  handleRequestPath(request.getPathWithoutQuery(), false);
	if (this->_serverConfig.isValidLocationPath(path))
	{
		LocationConfig myLocation = this->_serverConfig.getLocationConfigByPath(path);

		std::string interpreter;
		if (request.resolveCgiInterpreter(myLocation.getCgiHandler(), interpreter))
		{
			executeCgi(request, interpreter);
			return ;
		}

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

void HTTPResponse::executeCgi(HTTPRequest &request, const std::string &interpreter)
{
	std::string scriptPath = request.getPathWithoutQuery();
	if (!scriptPath.empty() && scriptPath[0] == '/')
		scriptPath.erase(0, 1);

	std::string fullPath = "www/" + scriptPath;
	struct stat fileStat;
	if (stat(fullPath.c_str(), &fileStat) != 0 || !S_ISREG(fileStat.st_mode))
		throw HTTPRequest::StatusException("404", "Page Not Found");
	if (!(fileStat.st_mode & S_IXUSR))
		throw HTTPRequest::StatusException("403", "Forbidden");

	std::string scriptDir = "www";
	std::string scriptBase = scriptPath;
	std::size_t slashPos = scriptPath.rfind('/');
	if (slashPos != std::string::npos)
	{
		scriptDir += "/" + scriptPath.substr(0, slashPos);
		scriptBase = scriptPath.substr(slashPos + 1);
	}

	int inPipe[2];
	int outPipe[2];
	if (pipe(inPipe) < 0 || pipe(outPipe) < 0)
		throw HTTPRequest::StatusException("500", "Internal Server Error");

	pid_t pid = fork();
	if (pid < 0)
	{
		close(inPipe[0]); close(inPipe[1]);
		close(outPipe[0]); close(outPipe[1]);
		throw HTTPRequest::StatusException("500", "Internal Server Error");
	}

	if (pid == 0)
	{
		dup2(inPipe[0], STDIN_FILENO);
		dup2(outPipe[1], STDOUT_FILENO);
		dup2(outPipe[1], STDERR_FILENO);
		close(inPipe[1]);
		close(outPipe[0]);
		close(inPipe[0]);
		close(outPipe[1]);
		chdir(scriptDir.c_str());
		char *argv[3];
		argv[0] = const_cast<char *>(interpreter.c_str());
		argv[1] = const_cast<char *>(scriptBase.c_str());
		argv[2] = NULL;

		std::vector<std::string> envVec;
		envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
		envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
		envVec.push_back("REQUEST_METHOD=" + request.getMethod());
		envVec.push_back("QUERY_STRING=" + request.getQueryString());
		envVec.push_back("SCRIPT_FILENAME=" + scriptPath);
		envVec.push_back("SCRIPT_NAME=" + request.getPathWithoutQuery());
		envVec.push_back("CONTENT_LENGTH=" + toString(request.getContentLength()));
		envVec.push_back("CONTENT_TYPE=" + request.getContentType());

		char **envp = new char *[envVec.size() + 1];
		for (std::size_t i = 0; i < envVec.size(); ++i)
		{
			envp[i] = new char[envVec[i].size() + 1];
			std::strcpy(envp[i], envVec[i].c_str());
		}
		envp[envVec.size()] = NULL;

		execve(argv[0], argv, envp);
		_exit(1);
	}

	close(inPipe[0]);
	std::string body = request.getBody();
	if (!body.empty())
		write(inPipe[1], body.c_str(), body.size());
	close(inPipe[1]);
	close(outPipe[1]);

	std::string cgiOutput;
	char buf[4096];
	time_t startTime = time(NULL);
	int const cgiTimeout = 5;

	while (true)
	{
		if (time(NULL) - startTime > cgiTimeout)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			close(outPipe[0]);
			throw HTTPRequest::StatusException("504", "Gateway Timeout");
		}
		ssize_t r = read(outPipe[0], buf, sizeof(buf));
		if (r > 0)
			cgiOutput.append(buf, r);
		else if (r == 0)
			break ;
		else if (errno == EAGAIN || errno == EINTR)
			continue ;
		else
			break ;
	}
	close(outPipe[0]);

	int wstatus;
	waitpid(pid, &wstatus, 0);

	this->_body = cgiOutput;
	this->_status = "200";
	this->_message = "OK";
}

void HTTPResponse::handleRessource(HTTPRequest &request)
{
	std::string ressource =  handleRequestPath(request.getPathWithoutQuery(), true);
	if (!ressource.empty() && ressource[0] == '/')
		ressource.erase(0, 1);
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
			throw HTTPRequest::StatusException("405", "Method Not Allowed");

		std::string interpreter;
		if (request.resolveCgiInterpreter(myLocation.getCgiHandler(), interpreter))
		{
			executeCgi(request, interpreter);
			return ;
		}

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

static void	delete_recursive(const char *filename);

void	HTTPResponse::handleDeleteMethod(HTTPRequest &request)

{
	std::string const	path = handleRequestPath(request.getPathWithoutQuery(), false);
	std::string const		filename = "www" + path;
	const char*				filename_c_str = filename.c_str();
	struct stat				st;
	LocationConfig 			location;

	try
	{
		location = this->_serverConfig.getLocationConfigByPath(path);
	}
	catch(const std::out_of_range& e)
	{
		location = this->_serverConfig.getLocationConfigByPath("/");
	}
	if (!this->_serverConfig.isValidLocationPath(path))
	{
		location = this->_serverConfig.getLocationConfigByPath("/");
		std::cout << "location path: " << location.getPath() << std::endl;
	} 
	if (!location.getReturn().empty())
	{
		std::string	newPath = location.getReturn();
		request.setPath(newPath);
		this->_status = "301";
		this->_message = "Moved Permanently";
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
