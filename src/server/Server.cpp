/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:10:24 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 18:04:01 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(ServerConfig &serverConfig) : _serverConfig(serverConfig)
{
    try
    {
        this->setupServer();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;

        this->closeListenSockets();
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

void Server::closeListenSockets()
{
    std::string pad(" ", 4);
    for (std::size_t i = 0; i < this->_listenSockets.size(); i++)
    {
        std::cout << pad << pad << MAGENTA "Closing listen [" << i << "]: " << this->_listenSockets.at(i) << DEFAULT << std::endl;
        close(this->_listenSockets.at(i));
    }
}

void Server::setupServer()
{
    std::vector<listenPair> listenSockets = this->_serverConfig.getListen();
    for (std::size_t i = 0; i < listenSockets.size(); i++)
    {
        // Create a new socket (AF_INET == IPv4, SOCK_STREM == TCP)
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            throw std::runtime_error(RED "socket() error" DEFAULT);

        this->_listenSockets.push_back(fd);

        // Allow socket addr to be reused (if not, have to wait 1-4min before restarting server)
        int on = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
            throw std::runtime_error(RED "setsockopt() error" DEFAULT);

        // Bind socket to listen host:port         
        struct sockaddr_in addr;
        memset(&(addr), 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(listenSockets.at(i).first);
        addr.sin_port = htons(listenSockets.at(i).second);

        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            handleBindError(this->_serverConfig.getListenStr().at(i), this->_serverConfig.getFileName());
            continue;
        }

        // Enable the socket to receive connexions (passive mode) 
        if (listen(fd, SOMAXCONN) < 0)
            throw std::runtime_error(RED "listen() error" DEFAULT);

        std::cout << GREEN + getTimeOfDay() + " [ok] : Opened socket \"" << this->_serverConfig.getListenStr().at(i);
        std::cout << "\" (as fd " << fd << ") sucessfully!" << DEFAULT << std::endl;
    }
}
