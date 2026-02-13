/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:25 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/19 20:03:31 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALSERVER_HPP
#define GLOBALSERVER_HPP

#include "structServer.h"
#include "HTTPRequest.hpp"
#include "Server.hpp"
#include "SessionManager.hpp"

class GlobalServer
{
public:
    GlobalServer(GlobalConfig &globalConfig);
    ~GlobalServer();

private:
    // Servers info
    std::map<int, ServerConfig> _serversConfig;

    // Internal info
    int _epfd;
    GlobalConfig &_globalConfig;
    std::vector<Server> _servers;
    std::map<int, ClientContext *> _clientContexts;
    SessionManager _sessionManager;

    // Main methods
    void setupGlobalServer();
    void loopServer();

    // Other methods
    void handleNewClientConnexion(int &serverFd);
    void handleReading(ClientContext *clientContext);
    void handleWriting(ClientContext *clientContext);
    void handleCloseConnexion(ClientContext *clientContext);
    void closeOldClientConnexions();
};

#endif
