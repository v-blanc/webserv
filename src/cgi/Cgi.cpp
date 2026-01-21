/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabokhar <yabokhar@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:58:49 by yabokhar          #+#    #+#             */
/*   Updated: 2026/01/13 14:58:52 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include <sys/epoll.h>
#include <sys/stat.h>

static bool setNonBlocking(int fd)
{
	int const flags = fcntl(fd, F_GETFL, 0);

	if (flags < 0)
		return (false);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return (false);
	return (true);
}

std::vector<std::string> buildCgiEnv(HTTPRequest const& req, std::string const& scriptFilename)
{
	std::vector<std::string> env;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("REQUEST_METHOD=" + req.getMethod());
	env.push_back("QUERY_STRING=" + req.getQueryString());
	env.push_back("SCRIPT_FILENAME=" + scriptFilename);
	env.push_back("SCRIPT_NAME=" + req.getPathWithoutQuery());
	env.push_back("CONTENT_LENGTH=" + toString(req.getContentLength()));
	env.push_back("CONTENT_TYPE=" + req.getContentType());
	return (env);
}

char** vectorToEnvp(std::vector<std::string> const& env)
{
	char** envp = new char *[env.size() + 1];

	for (std::size_t i = 0; i < env.size(); ++i)
	{
		envp[i] = new char[env[i].size() + 1];
		std::strcpy(envp[i], env[i].c_str());
	}
	envp[env.size()] = NULL;
	return (envp);
}

void freeEnvp(char** envp)
{
	if (!envp)
		return ;
	for (std::size_t i = 0; envp[i] != NULL; ++i)
		delete[] envp[i];
	delete[] envp;
}

CgiContext* launchCgi(
	HTTPRequest const& httpRequest,
	std::string const& interpreter,
	ClientContext* clientContext,
	int epfd
)
{
	int inPipe[2];
	int outPipe[2];

	if (pipe(inPipe) < 0 || pipe(outPipe) < 0)
		return (NULL);

	setNonBlocking(outPipe[0]);

	struct stat interpreterStat;
	if (stat(interpreter.c_str(), &interpreterStat) != 0)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (NULL);
	}

	if (!(interpreterStat.st_mode & S_IXUSR))
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (NULL);
	}

	std::string scriptFilename = httpRequest.getPathWithoutQuery();
	if (!scriptFilename.empty() && scriptFilename[0] == '/')
		scriptFilename.erase(0, 1);

	std::string scriptDir = "www";
	std::string scriptBase = scriptFilename;
	std::size_t slashPos = scriptFilename.rfind('/');
	if (slashPos != std::string::npos)
	{
		scriptDir += "/" + scriptFilename.substr(0, slashPos);
		scriptBase = scriptFilename.substr(slashPos + 1);
	}

	std::string fullPath = "www/" + scriptFilename;
	struct stat fileStat;
	if (stat(fullPath.c_str(), &fileStat) != 0)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (NULL);
	}

	if (!S_ISREG(fileStat.st_mode))
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (NULL);
	}

	if (!(fileStat.st_mode & S_IXUSR))
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (NULL);
	}

	std::vector<std::string> envVec = buildCgiEnv(httpRequest, scriptFilename);
	char** envp = vectorToEnvp(envVec);

	pid_t pid = fork();
	if (pid < 0)
	{
		freeEnvp(envp);
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (NULL);
	}

	if (!pid)
	{
		dup2(inPipe[0], STDIN_FILENO);
		dup2(outPipe[1], STDOUT_FILENO);
		dup2(outPipe[1], STDERR_FILENO);
		close(inPipe[1]);
		close(outPipe[0]);
		chdir(scriptDir.c_str());

		char *argv[3];
		argv[0] = const_cast<char *>(interpreter.c_str());
		argv[1] = const_cast<char *>(scriptBase.c_str());
		argv[2] = NULL;
		execve(argv[0], argv, envp);
		_exit(1);
	}
	freeEnvp(envp);
	close(inPipe[0]);
	std::string body = httpRequest.getBody();
	if (!body.empty())
		write(inPipe[1], body.c_str(), body.size());
	close(inPipe[1]);
	close(outPipe[1]);

	CgiContext *cgiCtx = new CgiContext;
	cgiCtx->fd = outPipe[0];
	cgiCtx->client = clientContext;
	cgiCtx->pid = pid;
	cgiCtx->startTime = time(NULL);
	cgiCtx->script = scriptFilename;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
	ev.data.ptr = cgiCtx;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, cgiCtx->fd, &ev) < 0)
	{
		close(cgiCtx->fd);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		delete cgiCtx;
		return (NULL);
	}
	return (cgiCtx);
}

