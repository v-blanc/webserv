/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:25 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/19 13:40:47 by vblanc           ###   ########.fr       */
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
    std::vector<Server> _servers;

    int _epfd;
    std::map<int, ClientContext *> _clientContexts;

    void setupGlobalServer();
    void loopServer();

    void handleNewClientConnexion(int &serverFd);
    void handleCloseConnexion(ClientContext *clientContext);
    void handleClientClosedConnexion(ClientContext *clientContext);
    void handleReading(int &clientFd);
    void handleWriting(int &clientFd); // TODO: Client fd ??
    void closeOldClientConnexions();
};

#endif
