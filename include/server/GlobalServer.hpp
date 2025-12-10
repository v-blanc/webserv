/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:25 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/10 14:45:06 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALSERVER_HPP
#define GLOBALSERVER_HPP

#include "structsServer.h"
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
    std::map<int, ConnexionState> _clientConnexions;

    void setupGlobalServer();
    void loopServer();

    void handleNewClientConnexion(int serverFd);
    void handleCloseConnexion(int clientFd); // Client fd ??
    void handleClientClosedConnexion(int clientFd);
    void handleReading(int clientFd);
    void closeOldClientConnexions();
};

#endif
