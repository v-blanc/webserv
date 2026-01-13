/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:25 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/11 21:23:53 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALSERVER_HPP
#define GLOBALSERVER_HPP

#include "structServer.h"
#include "HTTPRequest.hpp"
#include "Server.hpp"

class GlobalServer
{
public:
    GlobalServer(GlobalConfig &globalConfig);
    ~GlobalServer();

private:
    GlobalConfig &_globalConfig;
    std::map<int, ServerConfig> _serversConfig;
    std::vector<Server> _servers;

    int _epfd;
    std::map<int, ClientContext *> _clientContexts;

    void setupGlobalServer();
    void loopServer();

    void handleNewClientConnexion(int &serverFd);
    void handleCloseConnexion(ClientContext *clientContext);
    void handleReading(ClientContext *clientContext);
    void handleWriting(ClientContext *clientContext); // TODO: Client fd ??
    void closeOldClientConnexions();
};

#endif
