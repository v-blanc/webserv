/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:10:24 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/11 19:02:59 by vblanc           ###   ########.fr       */
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
    }
}

Server::~Server()
{
}

void Server::closeServerSockets()
{
    std::string pad(4, ' ');
    for (std::size_t i = 0; i < this->_serverSockets.size(); i++)
    {
        std::cout << pad << pad << MAGENTA "Closing server socket [" << i << "]: " << this->_serverSockets.at(i) << DEFAULT << std::endl;
        close(this->_serverSockets.at(i));
    }
    std::cout << std::endl;
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

        this->_serverSockets.push_back(fd);

        // Allow socket addr to be reused (if not, have to wait 1-4min before restarting server)
        int on = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
            throw std::runtime_error(RED "setsockopt() error" DEFAULT);

        // Bind socket to listen host:port
        struct sockaddr_in addr;
        memset(&(addr), 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = serverSockets.at(i).first;
        addr.sin_port = serverSockets.at(i).second;

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            handleBindError(this->_serverConfig.getListenStr().at(i), this->_serverConfig.getFileName());
            continue;
        }

        // Enable the socket to receive connexions (passive mode)
        if (listen(fd, SOMAXCONN) < 0)
            throw std::runtime_error(RED "listen() error" DEFAULT);

        // Add the fd to epoll (EPOLL_CTL_ADD) as EPOLLIN (server side socket)
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        ev.data.fd = fd;

        if (epoll_ctl(this->_epfd, EPOLL_CTL_ADD, fd, &ev))
            throw std::runtime_error(RED "epoll_ctl() error" DEFAULT);

        std::cout << GREEN + getTimeOfDay() + " [ok] : Opened socket \"" << this->_serverConfig.getListenStr().at(i);
        std::cout << "\" (as fd " << fd << ") sucessfully!" << DEFAULT << std::endl;
    }
}
