/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:08:33 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/11 21:24:01 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include "structServer.h"
#include "GlobalConfig.hpp"

class Server
{
public:
    Server(ServerConfig &serverConfig, int &epfd);
    ~Server();

    void closeServerSockets();
    bool hasListenFd(int fd) const { return (this->_serverContexts.count(fd) ^ 0); };

    // Getter
    ServerConfig getServerConfig() const { return this->_serverConfig; };
    std::map<int, ServerContext *> getServerContext() const { return this->_serverContexts; };

private:
    ServerConfig &_serverConfig;

    int &_epfd;
    std::map<int, ServerContext *> _serverContexts;

    void setupServer();
};

#endif
