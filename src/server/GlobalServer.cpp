/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:14 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/13 15:51:31 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"

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

static ClientContext *newClientContext(int clientSocket)
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

        ClientContext *clientContext = newClientContext(clientSocket);
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
        return;
    }

    std::cout << MAGENTA + getTimeOfDay() + " [debug] : Close connexion fd " << clientContext->fd << DEFAULT << std::endl;

    if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientContext->fd, NULL))
        throw std::runtime_error(RED "epoll_ctl() HERE error" DEFAULT);

    close(clientContext->fd);
    this->_clientContexts.erase(clientContext->fd);
    delete clientContext;
}

void GlobalServer::handleReading(ClientContext *clientContext)
{
    std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Read from fd " << clientContext->fd << DEFAULT << std::endl;

    ssize_t r;
    std::string request;
    char buf[RECV_BUFFER_SIZE];

    while (true)
    {
        r = recv(clientContext->fd, buf, RECV_BUFFER_SIZE, 0);

        if (r > 0)
        {
            for (int i = 0; i < r; i++)
                request.push_back(buf[i]);
        }
        else if (r == 0)
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
        // HTTPRequest httpRequest(request);
        // printHTTPRequest(httpRequest);

        // httpRequest.debugStandardReponse(clientContext->fd);
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
        delete this->_clientContexts.at(clientContextsToClose.at(i));
        this->_clientContexts.erase(clientContextsToClose.at(i));
    }
}
