/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:25 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/10 12:05:22 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALSERVER_HPP
#define GLOBALSERVER_HPP

#include "Server.hpp"
#include "structsServer.h"
#include <map>

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

    bool handleNewClientConnexion(int serverFd);
    void closeOldClientConnexions();
};

#endif
