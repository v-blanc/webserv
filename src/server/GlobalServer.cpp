/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:14 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/11 14:37:13 by vblanc           ###   ########.fr       */
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
    std::string pad(" ", 4);
    std::cout << MAGENTA BOLD "~GlobalServer():" DEFAULT << std::endl;

    for (std::map<int, ConnexionState>::iterator it = this->_clientConnexions.begin(); it != this->_clientConnexions.end(); it++)
    {
        std::cout << pad << MAGENTA "Closing client fd " << it->first << DEFAULT << std::endl;
        close(it->first);
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
        int n = epoll_wait(this->_epfd, events, MAX_EPOLL_WAIT_EVENTS, 0);

        this->closeOldClientConnexions();

        if (n == -1)
            continue;

        for (int i = 0; i < n; i++) // Loop over events
        {
            for (std::size_t j = 0; j < this->_servers.size(); j++) // Loop over servers
            {
                std::vector<int> serverSockets = this->_servers.at(j).getServerSockets();

                for (std::size_t k = 0; k < serverSockets.size(); k++) // Loop over listen sockets
                    if (events[i].data.fd == serverSockets.at(k) && events[i].events & EPOLLIN)
                        this->handleNewClientConnexion(events[i].data.fd);

                if (events[i].events & (EPOLLERR | EPOLLHUP))
                    this->handleCloseConnexion(events[i].data.fd);
                else if (events[i].events & EPOLLRDHUP)
                    this->handleClientClosedConnexion(events[i].data.fd);
                else // EPOLLIN and/or EPOLLOUT
                {
                    if (events[i].events & EPOLLIN) // Read until EAGAIN
                        this->handleReading(events[i].data.fd);
                    if (events[i].events & EPOLLOUT) // Write
                        this->handleWriting(events[i].data.fd);
                }
            }
        }
    }
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

        ConnexionState connexionState;
        connexionState.fd = clientSocket;
        connexionState.lastActive = time(NULL);
        connexionState.keepAlive = false;

        this->_clientConnexions[clientSocket] = connexionState;

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        ev.data.fd = clientSocket;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, clientSocket, &ev))
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "errno == EAGAIN || EWOULDBLOCK" << std::endl;
                break;
            }

            this->_clientConnexions.erase(clientSocket);
            close(clientSocket);
            std::cerr << RED "epoll_ctl() error" DEFAULT << std::endl;
            return;
        }

        std::cout << GREEN + getTimeOfDay() + " [ok] : Accepted new connexion from port ‘" << ntohs(clientAddr.sin_port) << "’ (as fd " << clientSocket << ")" DEFAULT << std::endl;
    }
}

void GlobalServer::handleCloseConnexion(int &clientFd)
{
    if (this->_clientConnexions.count(clientFd) == 0)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : handleCloseConnexion(): Trying to close client fd " << clientFd << " but is already closed" DEFAULT << std::endl;
        return;
    }

    std::cout << MAGENTA + getTimeOfDay() + " [debug] : Close connexion fd " << clientFd << DEFAULT << std::endl;

    if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientFd, NULL))
        throw std::runtime_error(RED "epoll_ctl() HERE error" DEFAULT);

    close(clientFd);
    this->_clientConnexions.erase(clientFd);
}

void GlobalServer::handleClientClosedConnexion(int &clientFd)
{
    if (this->_clientConnexions.count(clientFd) == 0)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : handleClientClosedConnexion(): Trying to close client fd " << clientFd << " but is already closed" DEFAULT << std::endl;
        return;
    }

    std::cout << MAGENTA + getTimeOfDay() + " [debug] : Client fd " << clientFd << " closed connexion" << DEFAULT << std::endl;

    if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientFd, NULL))
        throw std::runtime_error(RED "epoll_ctl() TEST error" DEFAULT);

    close(clientFd);
    this->_clientConnexions.erase(clientFd);
}

void GlobalServer::handleReading(int &clientFd)
{
    ssize_t r;
    std::string request;
    char buf[10];

    while ((r = recv(clientFd, buf, 10, 0)) > 0)
    {
        for (int i = 0; i < r; i++)
            request.push_back(buf[i]);
    }

    if (request.empty())
        return;

    std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Read from fd " << clientFd << DEFAULT << std::endl;

    try
    {
        HTTPRequest httpRequest(request);
        // printHTTPRequest(httpRequest);

        // TODO: send a custom message, for now just debug
        std::string sendBuf = "HTTP/1.1 200 OK\r\nLocation: http://localhost:8080/\r\nContent-Length: ";

        std::string fileName = "./docs/webserv_page" + httpRequest.getPath();

        if (httpRequest.getPath() == "/")
            fileName.append("index.html");

        // Avoid Chrome duplicates
        // if (fileName.find("favicon.ico") != std::string::npos)
        //     return;

        if (isInvalidPath(fileName))
        {
            std::cerr << "Invalid path" << std::endl;
            return;
        }

        std::string content = getLocalFileContent(fileName);

        sendBuf.append(to_string(content.size()));
        sendBuf.append("\r\n\r\n");
        sendBuf.append(content);

        send(clientFd, sendBuf.c_str(), sendBuf.size(), MSG_NOSIGNAL);

        std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Response to request sent to fd " << clientFd << DEFAULT << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void GlobalServer::handleWriting(int &clientFd) // TODO: Client fd ?
{
    std::cout << "Write socket " << clientFd << std::endl;
}

void GlobalServer::closeOldClientConnexions()
{
    // Close client fds after 4 seconds
    time_t secondsBeforeClosing = 4;
    std::vector<int> clientConnexionsToClose;

    for (std::map<int, ConnexionState>::iterator it = this->_clientConnexions.begin(); it != this->_clientConnexions.end(); it++)
    {
        if ((time(NULL) - it->second.lastActive) > secondsBeforeClosing)
            clientConnexionsToClose.push_back(it->first);
    }

    for (std::size_t i = 0; i < clientConnexionsToClose.size(); i++)
    {
        std::cout << MAGENTA + getTimeOfDay() + " [debug] : Closing old client fd " << clientConnexionsToClose.at(i) << DEFAULT << std::endl;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, clientConnexionsToClose.at(i), NULL))
            throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);

        close(clientConnexionsToClose.at(i));
        this->_clientConnexions.erase(clientConnexionsToClose.at(i));
    }
}
