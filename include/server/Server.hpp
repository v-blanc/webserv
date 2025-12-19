/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:08:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/19 13:40:44 by vblanc           ###   ########.fr       */
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

    // Getter
    std::map<int, ServerContext *> getServerSockets() const { return this->_serverContexts; };

private:
    ServerConfig &_serverConfig;

    int &_epfd;
    std::map<int, ServerContext *> _serverContexts;

    void setupServer();
};

#endif
