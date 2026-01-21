/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabokhar <yabokhar@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 18:45:00 by yabokhar          #+#    #+#             */
/*   Updated: 2026/01/21 18:45:00 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "Cgi.hpp"
#include "utils.hpp"
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>

#define CGI_TIMEOUT_SECONDS 5

void unchunkBody(std::string &body)
{
	std::string		unchunked;
	std::size_t		pos;

	pos = 0;
	while (pos < body.size())
	{
		std::size_t lineEnd = body.find("\r\n", pos);
		if (lineEnd == std::string::npos)
			break ;
		std::string chunkSizeStr = body.substr(pos, lineEnd - pos);
		std::size_t chunkSize = std::strtoul(chunkSizeStr.c_str(), NULL, 16);
		pos = lineEnd + 2;
		if (!chunkSize)
			break ;
		unchunked.append(body, pos, chunkSize);
		pos += chunkSize + 2;
	}
	body = unchunked;
}

static bool pathStartsWithLocation(std::string const &path, std::string const &location)
{
	if (location.empty())
		return (false);
	if (location == "/")
		return (true);
	if (path.size() < location.size())
		return (false);
	if (path.compare(0, location.size(), location) != 0)
		return (false);
	if (path.size() == location.size())
		return (true);
	return (path[location.size()] == '/');
}

static LocationConfig const *findBestLocation(std::vector<LocationConfig> const &locations, std::string const &path)
{
	LocationConfig const	*best;
	std::size_t				bestLen;

	best = NULL;
	bestLen = 0;
	for (std::size_t i = 0; i < locations.size(); ++i)
	{
		std::string const &locPath = locations[i].getPath();
		if (pathStartsWithLocation(path, locPath) && locPath.size() >= bestLen)
		{
			best = &locations[i];
			bestLen = locPath.size();
		}
	}
	return (best);
}

static ServerConfig const &pickServerConfig(std::vector<ServerConfig> const &servers, std::vector<Server> const &runtimeServers, int serverFd)
{
	for (std::size_t i = 0; i < runtimeServers.size() && i < servers.size(); ++i)
	{
		if (runtimeServers[i].hasListenFd(serverFd))
			return (servers[i]);
	}
	return (servers.at(0));
}

bool tryLaunchCgi(
	HTTPRequest const& httpRequest,
	GlobalConfig &globalConfig,
	std::vector<Server> const &servers,
	ClientContext *clientContext,
	int epfd
)
{
	std::vector<ServerConfig> serverConfigs = globalConfig.getServerConfig();
	if (serverConfigs.empty())
		return (false);

	ServerConfig const &server = pickServerConfig(serverConfigs, servers, clientContext->serverFd);
	std::vector<LocationConfig> const locations = server.getLocationConfig();
	LocationConfig const *bestLoc = findBestLocation(locations, httpRequest.getPathWithoutQuery());

	std::vector<stringPair> cgiHandlers;
	if (bestLoc != NULL)
		cgiHandlers = bestLoc->getCgiHandler();
	else
		cgiHandlers = server.getCgiHandler();

	std::string interpreter;
	if (httpRequest.resolveCgiInterpreter(cgiHandlers, interpreter))
	{
		CgiContext *cgiCtx = launchCgi(httpRequest, interpreter, clientContext, epfd);
		if (!cgiCtx)
		{
			clientContext->sendBuffer = "HTTP/1.1 500 Internal Server Error\r\n";
			clientContext->sendBuffer += "Content-Type: text/plain\r\n";
			clientContext->sendBuffer += "Connection: close\r\n\r\n";
			clientContext->sendBuffer += "CGI launch failed\n";
			clientContext->state = READY_TO_SEND;
			return (true);
		}
		return (true);
	}
	return (false);
}

static void handleCgiTimeout(CgiContext *cgiContext, int epfd)
{
	kill(cgiContext->pid, SIGKILL);
	waitpid(cgiContext->pid, NULL, 0);

	ClientContext *client = cgiContext->client;
	
	client->sendBuffer = "HTTP/1.1 504 Gateway Timeout\r\n";
	client->sendBuffer += "Content-Type: text/plain\r\n";
	client->sendBuffer += "Connection: close\r\n\r\n";
	client->sendBuffer += "CGI timeout\n";
	client->state = READY_TO_SEND;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
	ev.data.ptr = client;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client->fd, &ev);

	epoll_ctl(epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL);
	close(cgiContext->fd);
	delete cgiContext;
}

static void handleCgiSuccess(CgiContext *cgiContext, int epfd)
{
	int status;
	waitpid(cgiContext->pid, &status, 0);

	ClientContext *client = cgiContext->client;

	std::string body = cgiContext->cgiOut;
	client->sendBuffer = "HTTP/1.1 200 OK\r\n";
	client->sendBuffer += "Content-Type: text/plain\r\n";
	client->sendBuffer += "Content-Length: " + toString(body.size()) + "\r\n";
	client->sendBuffer += "Connection: close\r\n\r\n";
	client->sendBuffer += body;
	client->state = READY_TO_SEND;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
	ev.data.ptr = client;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client->fd, &ev);

	epoll_ctl(epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL);
	close(cgiContext->fd);
	delete cgiContext;
}

static void handleCgiError(CgiContext *cgiContext, int epfd)
{
	waitpid(cgiContext->pid, NULL, WNOHANG);

	ClientContext *client = cgiContext->client;

	client->sendBuffer = "HTTP/1.1 500 Internal Server Error\r\n";
	client->sendBuffer += "Content-Type: text/plain\r\n";
	client->sendBuffer += "Connection: close\r\n\r\n";
	client->sendBuffer += "CGI execution error\n";
	client->state = READY_TO_SEND;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
	ev.data.ptr = client;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client->fd, &ev);

	epoll_ctl(epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL);
	close(cgiContext->fd);
	delete cgiContext;
}

void handleCgiEvent(struct epoll_event &event, int epfd)
{
	CgiContext *cgiContext = static_cast<CgiContext *>(event.data.ptr);

	if ((time(NULL) - cgiContext->startTime) > CGI_TIMEOUT_SECONDS)
	{
		handleCgiTimeout(cgiContext, epfd);
		return;
	}

	if (event.events & (EPOLLIN | EPOLLHUP | EPOLLRDHUP))
	{
		char buf[4096];
		ssize_t r;

		while ((r = read(cgiContext->fd, buf, sizeof(buf))) > 0)
			cgiContext->cgiOut.append(buf, r);

		if (r == 0 || (event.events & (EPOLLHUP | EPOLLRDHUP)))
			handleCgiSuccess(cgiContext, epfd);
		else if (r < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        	handleCgiError(cgiContext, epfd);
	}
}