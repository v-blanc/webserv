/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 15:08:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/10 13:24:03 by vblanc           ###   ########.fr       */
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

    void closeServerSockets();

    // Getter
    std::vector<int> getServerSockets() const { return this->_serverSockets; };

private:
    ServerConfig &_serverConfig;

    int &_epfd;
    std::vector<int> _serverSockets;

    void setupServer();
};

#endif
