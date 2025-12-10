/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:14 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/10 12:15:13 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"

bool keepRuningServer = true;

static void sigHandler(int signal)
{
    if (signal == SIGINT)
        keepRuningServer = false;
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

GlobalServer::~GlobalServer() // Close every server fd's here
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
        this->_servers.at(i).closeListenSockets();
    }
    std::cout << DEFAULT;

    std::cout << pad << MAGENTA "Closing epoll fd (this->_epfd)" DEFAULT << std::endl;
    close(this->_epfd);

    signal(SIGINT, SIG_DFL);
}

void GlobalServer::setupGlobalServer()
{
    this->_epfd = epoll_create(1); // TODO: handle the parameter
    if (this->_epfd == -1)
        throw std::runtime_error(RED "epoll_create() error" DEFAULT);

    std::vector<ServerConfig> serverConfig = this->_globalConfig.getServerConfig();
    for (std::size_t i = 0; i < serverConfig.size(); i++)
    {
        std::cout << BLUE + getTimeOfDay() + " [info] : Trying to create server ‘" ITALIC + serverConfig.at(i).getServerName().at(0) + DEFAULT BLUE "’..." DEFAULT << std::endl;
        _servers.push_back(Server(serverConfig.at(i), this->_epfd));
    }
}

void GlobalServer::loopServer()
{
    while (keepRuningServer)
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
                std::vector<int> listenSockets = this->_servers.at(j).getListenSockets();

                for (std::size_t k = 0; k < listenSockets.size(); k++) // Loop over listen sockets
                {
                    // Server fds
                    if (events[i].data.fd == listenSockets.at(k) && events[i].events & EPOLLIN) // Connection to socket => accept()
                    {
                        if (this->handleNewClientConnexion(events[i].data.fd))
                            continue;
                    }
                    // Client fds
                    else if (events[i].events & (EPOLLERR | EPOLLHUP)) // Close connexion
                    {
                        std::cout << "Closing socket " << events[i].data.fd << std::endl;

                        if (epoll_ctl(this->_epfd, EPOLL_CTL_DEL, listenSockets.at(k), NULL))
                            throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);

                        close(listenSockets.at(k));
                    }
                    else if (events[i].events & EPOLLRDHUP) // Client closed connexion
                    {
                        // std::cout << "Client closed socket " << events[i].data.fd << std::endl;
                    }
                    else if (events[i].events & EPOLLIN) // Read until EAGAIN
                    {
                        // std::cout << "Read socket " << events[i].data.fd << std::endl;
                    }
                    else if (events[i].events & EPOLLOUT) // Write
                    {
                        // std::cout << "Write socket " << events[i].data.fd << std::endl;
                    }
                }
            }
        }

        // break;
    }
}

bool GlobalServer::handleNewClientConnexion(int serverFd)
{
    std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Trying a new connection to server fd " << serverFd << DEFAULT << std::endl;

    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while ((clientSocket = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrSize)) != -1)
    {
        if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
        {
            std::cout << ">> HERE\n";
            close(clientSocket);
            return true;
        }

        ConnexionState connexionState;
        connexionState.fd = clientSocket;
        connexionState.lastActive = time(NULL);
        connexionState.keepAlive = false;

        this->_clientConnexions[clientSocket] = connexionState;

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP /* | EPOLLET */;
        ev.data.fd = clientSocket;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, clientSocket, &ev))
        {
            this->_clientConnexions.erase(clientSocket);
            close(clientSocket);
            std::cerr << RED "epoll_ctl() error" DEFAULT << std::endl;
            return true;
        }

        std::cout << GREEN + getTimeOfDay() + " [ok] : Accepted new connexion from port ‘" << ntohs(clientAddr.sin_port) << "’ (as fd " << clientSocket << ")" DEFAULT << std::endl;
    }

    if (errno == (EAGAIN | EWOULDBLOCK))
        return (true);

    return (false);
}

void GlobalServer::closeOldClientConnexions()
{
    // Close client fds after 4 seconds
    std::vector<int> clientConnexionsToClose;
    time_t secondsBeforeClosing = 4;

    for (std::map<int, ConnexionState>::iterator it = this->_clientConnexions.begin(); it != this->_clientConnexions.end(); it++)
    {
        if ((time(NULL) - it->second.lastActive) > secondsBeforeClosing)
            clientConnexionsToClose.push_back(it->first);
    }

    for (std::size_t i = 0; i < clientConnexionsToClose.size(); i++)
    {
        std::cout << YELLOW DARKEN + getTimeOfDay() + " [debug] : Closing client fd " << clientConnexionsToClose.at(i) << DEFAULT << std::endl;
        close(clientConnexionsToClose.at(i));
        this->_clientConnexions.erase(clientConnexionsToClose.at(i));
    }
}
