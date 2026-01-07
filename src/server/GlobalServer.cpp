/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:14 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/19 16:49:18 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"
#include <fcntl.h>
#include <sys/wait.h>

#define CGI_TIMEOUT_SECONDS 5

static bool setNonBlocking(int fd)

{
	int const flags = fcntl(fd, F_GETFL, 0);

	if (flags < 0)
		return (false);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return (false);
	return (true);
}

static std::vector<std::string> buildCgiEnv(HTTPRequest const& req, std::string const& scriptFilename)

{
    std::vector<std::string> env;

    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("QUERY_STRING=" + req.getQueryString());
    env.push_back("SCRIPT_FILENAME=" + scriptFilename);
    env.push_back("SCRIPT_NAME=" + req.getPathWithoutQuery());
    env.push_back("CONTENT_LENGTH=" + toString(req.getContentLength()));
    return (env);
}

static char** vectorToEnvp(std::vector<std::string> const& env)

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

static void freeEnvp(char** envp)
{
    if (!envp)
        return ;
    for (std::size_t i = 0; envp[i] != NULL; ++i)
        delete[] envp[i];
    delete[] envp;
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
    LocationConfig const    *best;
    std::size_t             bestLen;

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

bool keepRunningServer = true;

static void sigHandler(int signal)
{
    if (signal == SIGINT)
        keepRunningServer = false;
}

GlobalServer::GlobalServer(GlobalConfig &globalConfig) : _globalConfig(globalConfig)
{
    try
    {
        signal(SIGINT, sigHandler);

        this->setupGlobalServer();
        this->loopServer();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

GlobalServer::~GlobalServer() // Close every fd's here
{
    std::string pad(4, ' ');
    std::cout << MAGENTA BOLD "~GlobalServer():" DEFAULT << std::endl;

    for (std::map<int, ClientContext *>::iterator it = this->_clientContexts.begin(); it != this->_clientContexts.end(); it++)
    {
        std::cout << pad << MAGENTA "Closing client fd " << it->first << DEFAULT << std::endl;
        close(it->first);
        delete it->second;
    }
    std::cout << std::endl;

    for (std::size_t i = 0; i < this->_servers.size(); i++)
    {
        std::cout << pad << MAGENTA "For server \'" << this->_globalConfig.getServerConfig().at(i).getServerName().at(0) << "\' [" << i << "]:" DEFAULT << std::endl;
        this->_servers.at(i).closeServerSockets();
    }
    std::cout << DEFAULT;

    std::cout << pad << MAGENTA "Closing epoll fd (this->_epfd)" DEFAULT << std::endl;
    close(this->_epfd);

    signal(SIGINT, SIG_DFL);
}

void GlobalServer::setupGlobalServer()
{
    this->_epfd = epoll_create(1);
    if (this->_epfd < 0)
        throw std::runtime_error(RED "epoll_create() error" DEFAULT);

    std::vector<ServerConfig> serverConfig = this->_globalConfig.getServerConfig();
    for (std::size_t i = 0; i < serverConfig.size(); i++)
    {
        std::cout << BLUE + getTimeOfDay() + " [info] : Trying to create server ‘" ITALIC + serverConfig.at(i).getServerName().at(0) + DEFAULT BLUE "’..." DEFAULT << std::endl;
        _servers.push_back(Server(serverConfig.at(i), this->_epfd));
        std::cout << GREEN + getTimeOfDay() + " [ok] : Created server ‘" ITALIC + serverConfig.at(i).getServerName().at(0) + DEFAULT GREEN "’ sucessfully!" DEFAULT << std::endl;
    }

    std::cout << GREEN + getTimeOfDay() + " [ok] : Created server(s) sucessfully!" DEFAULT << std::endl;
}

void GlobalServer::loopServer()
{
    std::cout << BLUE + getTimeOfDay() + " [info] : Waiting events..." DEFAULT << std::endl;

    while (keepRunningServer)
    {
        struct epoll_event events[MAX_EPOLL_WAIT_EVENTS];
        int n = epoll_wait(this->_epfd, events, MAX_EPOLL_WAIT_EVENTS, 100);

        this->closeOldClientConnexions();

        if (n == -1)
            continue;

        for (int i = 0; i < n; i++)
        {
            EpollContext *context = static_cast<EpollContext *>(events[i].data.ptr);

            if (ServerContext *serverContext = dynamic_cast<ServerContext *>(context))
            {
                if (events[i].events & EPOLLIN)
                    this->handleNewClientConnexion(serverContext->fd);
                // TODO: Needed ??
                // if (events[i].events & EPOLLOUT) // Write
                //     this->handleWriting(serverContext);
            }
            else if (CgiContext *cgiContext = dynamic_cast<CgiContext *>(context))
            {
            if ((time(NULL) - cgiContext->startTime) > CGI_TIMEOUT_SECONDS)
            {
                kill(cgiContext->pid, SIGKILL);
                waitpid(cgiContext->pid, NULL, WNOHANG);

                std::string resp = "HTTP/1.1 504 Gateway Timeout\r\n";
                resp += "Content-Type: text/plain\r\n";
                resp += "Connection: close\r\n\r\n";
                resp += "CGI timeout\n";
                send(cgiContext->client->fd, resp.c_str(), resp.size(), MSG_NOSIGNAL);

                epoll_ctl(this->_epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL);
                close(cgiContext->fd);
                ClientContext *client = cgiContext->client;
                delete cgiContext;
                this->handleCloseConnexion(client);
                continue;
            }
                if (events[i].events & EPOLLIN)
                {
                    char buf[4096];
                    ssize_t r;


                    while ((r = read(cgiContext->fd, buf, sizeof(buf))) > 0)
                        cgiContext->client->cgiOut.append(buf, r);
                    if (r == 0)
                    {
                        int status;
                        waitpid(cgiContext->pid, &status, WNOHANG);

                        std::string body = cgiContext->client->cgiOut;
                        std::string resp = "HTTP/1.1 200 OK\r\n";
                        resp += "Content-Type: text/plain\r\n";
                        resp += "Content-Length: " + toString(body.size()) + "\r\n";
                        resp += "Connection: close\r\n\r\n";
                        resp += body;
                        send(cgiContext->client->fd, resp.c_str(), resp.size(), MSG_NOSIGNAL);

                        epoll_ctl(this->_epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL);
                        close(cgiContext->fd);
                        ClientContext *client = cgiContext->client;
                        delete cgiContext;
                        this->handleCloseConnexion(client);
                    }
                else if (r < 0)
                {
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        epoll_ctl(this->_epfd, EPOLL_CTL_DEL, cgiContext->fd, NULL);
                        close(cgiContext->fd);
                        ClientContext *client = cgiContext->client;
                        delete cgiContext;
                        this->handleCloseConnexion(client);
                    }
                }
                }
            }
            else if (ClientContext *clientContext = dynamic_cast<ClientContext *>(context))
            {
                if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
                    this->handleCloseConnexion(clientContext);
                else
                {
                    if (events[i].events & EPOLLIN) // Read until EAGAIN
                        this->handleReading(clientContext);
                    if (events[i].events & EPOLLOUT) // Write
                        this->handleWriting(clientContext);
                }
            }
        }
    }
}

static ClientContext *newClientContext(int clientSocket, int serverFd)
{
    ClientContext *clientContext;

    try
    {
        clientContext = new ClientContext;
    }
    catch (const std::exception &e)
    {
        std::cerr << RED + getTimeOfDay() + " [emerg] : Unexpected error during `new`: \"" << e.what() << "\"" << DEFAULT << std::endl;
        return (NULL);
    }

    clientContext->fd = clientSocket;
    clientContext->lastActive = time(NULL);
    clientContext->keepAlive = false;
    clientContext->serverFd = serverFd;
    clientContext->cgiOut.clear();
    return (clientContext);
}

void GlobalServer::handleNewClientConnexion(int &serverFd)
{
    std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Trying a new connection to server fd " << serverFd << DEFAULT << std::endl;

    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while ((clientSocket = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrSize)) != -1)
    {
        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
        {
            close(clientSocket);
            return;
        }

        ClientContext *clientContext = newClientContext(clientSocket, serverFd);
        if (clientContext == NULL)
        {
            close(clientSocket);
            continue;
        }

        this->_clientContexts[clientSocket] = clientContext;

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        ev.data.ptr = clientContext;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, clientSocket, &ev))
        {
            this->_clientContexts.erase(clientSocket);
            close(clientSocket);
            std::cerr << RED "epoll_ctl() error" DEFAULT << std::endl;
            return;
        }

        std::cout << GREEN + getTimeOfDay() + " [ok] : Accepted new connexion from port ‘" << ntohs(clientAddr.sin_port) << "’ (as fd " << clientSocket << ")" DEFAULT << std::endl;
    }
}

void GlobalServer::handleCloseConnexion(ClientContext *clientContext)
{
    if (this->_clientContexts.count(clientContext->fd) == 0)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : Trying to close client fd " << clientContext->fd << " but is already closed" DEFAULT << std::endl;
        return ;
    }

    std::cout << MAGENTA + getTimeOfDay() + " [debug] : Close connexion fd " << clientContext->fd << DEFAULT << std::endl;

    if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientContext->fd, NULL))
        throw (std::runtime_error(RED "epoll_ctl() HERE error" DEFAULT));

    close(clientContext->fd);
    this->_clientContexts.erase(clientContext->fd);
    delete clientContext;
}

void GlobalServer::handleReading(ClientContext *clientContext)
{
    ssize_t     r;
    std::string request;
    char        buf[RECV_BUFFER_SIZE];

    while (true)
    {
        r = recv(clientContext->fd, buf, RECV_BUFFER_SIZE, 0);
        if (r > 0)
        {
            for (int i = 0; i < r; i++)
                request.push_back(buf[i]);
        }
        else if (!r)
            break;
        else
        {
            if (errno == EAGAIN)
                break;
            if (errno == EINTR)
            {
                std::cout << "errno == EINTR" << std::endl;
                continue;
            }
            else
            {
                std::cout << "error ? r=" << r << " errno = " << errno << std::endl;
                return;
            }
        }
    }

    if (request.empty())
    {
        std::cout << "Empty request" << std::endl;
        return;
    }

    try
    {
        HTTPRequest httpRequest(request);
        // printHTTPRequest(httpRequest);

        std::vector<ServerConfig> servers = this->_globalConfig.getServerConfig();
        if (!servers.empty())
        {
            ServerConfig const &server = pickServerConfig(servers, this->_servers, clientContext->serverFd);
            std::string serverName = "<unknown>";
            if (!server.getServerName().empty())
                serverName = server.getServerName().at(0);
            std::string listenStr = "<unknown>";
            if (!server.getListenStr().empty())
                listenStr = server.getListenStr().at(0);
            std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Read from fd " << clientContext->fd
                    << " (server='" << serverName << "' listen='" << listenStr << "')" << DEFAULT << std::endl;
        }
        else
            std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Read from fd " << clientContext->fd << DEFAULT << std::endl;
        if (servers.empty())
        {
            httpRequest.debugStandardReponse(clientContext->fd);
            return ;
        }

        ServerConfig const &server = pickServerConfig(servers, this->_servers, clientContext->serverFd);
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
            int inPipe[2];
            int outPipe[2];

            if (pipe(inPipe) < 0 || pipe(outPipe) < 0)
            {
                httpRequest.debugStandardReponse(clientContext->fd);
                return ;
            }

            setNonBlocking(outPipe[0]);
            std::string                 scriptFilename = httpRequest.getPathWithoutQuery();
            if (!scriptFilename.empty() && scriptFilename[0] == '/')
                scriptFilename.erase(0, 1);
            std::string                 scriptDir = "www";
            std::string                 scriptBase = scriptFilename;
            std::size_t                 slashPos = scriptFilename.rfind('/');
            if (slashPos != std::string::npos)
            {
                scriptDir += "/" + scriptFilename.substr(0, slashPos);
                scriptBase = scriptFilename.substr(slashPos + 1);
            }
            std::vector<std::string>    envVec = buildCgiEnv(httpRequest, scriptFilename);
            char                        **envp = vectorToEnvp(envVec);

            pid_t pid = fork();
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
            epoll_ctl(this->_epfd, EPOLL_CTL_ADD, cgiCtx->fd, &ev);
            return ;
        }
        httpRequest.debugResponseWithCgiHandlers(clientContext->fd, cgiHandlers);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void GlobalServer::handleWriting(ClientContext *clientContext) // TODO: Client fd ?
{
    std::cout << "Write socket " << clientContext->fd << std::endl;
}

void GlobalServer::closeOldClientConnexions()
{
    std::vector<int> clientContextsToClose;

    for (std::map<int, ClientContext *>::iterator it = this->_clientContexts.begin(); it != this->_clientContexts.end(); it++)
    {
        if ((time(NULL) - it->second->lastActive) > TIMEOUT_OLD_CONNEXIONS)
            clientContextsToClose.push_back(it->first);
    }

    for (std::size_t i = 0; i < clientContextsToClose.size(); i++)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : Closing old client fd " << clientContextsToClose.at(i) << DEFAULT << std::endl;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientContextsToClose.at(i), NULL))
            throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);

        close(clientContextsToClose.at(i));
        delete (this->_clientContexts.at(clientContextsToClose.at(i)));
        this->_clientContexts.erase(clientContextsToClose.at(i));
    }
}
