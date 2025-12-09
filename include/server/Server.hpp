/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:08:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/09 20:36:47 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include "GlobalConfig.hpp"

class Server
{
public:
    Server(ServerConfig &serverConfig, int &epfd);
    ~Server();

    void closeListenSockets();

    // Getter
    std::vector<int> getListenSockets() const { return this->_listenSockets; };

private:
    ServerConfig &_serverConfig;

    int &_epfd;
    std::vector<int> _listenSockets;

    void setupServer();
};

#endif
