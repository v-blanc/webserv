/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:10:24 by vblanc            #+#    #+#             */
/*   Updated: 2026/02/25 14:44:10 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(ServerConfig &serverConfig, int &epfd) : _serverConfig(serverConfig), _epfd(epfd)
{
    try
    {
        this->setupServer();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;

        this->closeServerSockets();
        throwMajorIssueCreatingServer(this->_serverConfig.getServerName().at(0));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        this->closeServerSockets();
        throwMajorIssueCreatingServer(this->_serverConfig.getServerName().at(0));
    }
}

Server::~Server()
{
}

void Server::closeServerSockets()
{
    std::string pad(4, ' ');
    for (std::map<int, ServerContext *>::iterator it = this->_serverContexts.begin(); it != this->_serverContexts.end(); it++)
    {
        std::cout << pad << pad << MAGENTA "Closing server socket: " << it->first << DEFAULT << std::endl;
        close(it->first);
        if (it->second != NULL)
            delete it->second;
    }
    std::cout << std::endl;
}

static ServerContext *newServerContext(int &fd)
{
    ServerContext *serverContext;

    try
    {
        serverContext = new ServerContext;
    }
    catch (const std::exception &e)
    {
        std::cerr << RED + getTimeOfDay() + " [emerg] : Unexpected error during `new`: \"" << e.what() << "\"" << DEFAULT << std::endl;
        return (NULL);
    }

    serverContext->fd = fd;

    return (serverContext);
}

void Server::setupServer()
{
    std::vector<listenPair> serverSockets = this->_serverConfig.getListen();

    for (std::size_t i = 0; i < serverSockets.size(); i++)
    {
        // Create a new socket (AF_INET == IPv4, SOCK_STREM == TCP, SOCK_NONBLOCK == Non Blocking Socket, SOCK_CLOEXEC == Close inside execve() if sucess)
        int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if (fd < 0)
            throw std::runtime_error(RED "socket() error" DEFAULT);

        this->_serverContexts[fd] = NULL;

        // Allow socket addr to be reused (if not, have to wait 1-4min before restarting server)
        int on = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
            throw std::runtime_error(RED "setsockopt() error" DEFAULT);

        // Bind socket to listen host:port
        struct sockaddr_in addr;
        memset(&(addr), 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = serverSockets.at(i).second;

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            handleBindError(this->_serverConfig.getListenStr().at(i), this->_serverConfig.getFileName());
            continue;
        }

        // Enable the socket to receive connexions (passive mode)
        if (listen(fd, SOMAXCONN) < 0)
            throw std::runtime_error(RED "listen() error" DEFAULT);

        ServerContext *serverContext = newServerContext(fd);

        if (serverContext == NULL)
            throw std::runtime_error(RED + getTimeOfDay() + " [emerg] : Failed to create server ‘" ITALIC + this->_serverConfig.getServerName().at(0) + DEFAULT RED "’ (fd " + toString(fd) + ")" DEFAULT);

        this->_serverContexts[fd] = serverContext;

        // Add the fd to epoll (EPOLL_CTL_ADD) as EPOLLIN (server side socket)
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP;
        ev.data.ptr = serverContext;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, fd, &ev))
            throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);

        std::cout << GREEN + getTimeOfDay() + " [ok] : Opened socket \"" << this->_serverConfig.getListenStr().at(i);
        std::cout << "\" (as fd " << fd << ") sucessfully!" << DEFAULT << std::endl;
    }
}
