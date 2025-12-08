/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:14 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 18:05:58 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"

GlobalServer::GlobalServer(GlobalConfig &globalConfig) : _globalConfig(globalConfig)
{
    try
    {
        std::vector<ServerConfig> serverConfig = this->_globalConfig.getServerConfig();
        for (std::size_t i = 0; i < serverConfig.size(); i++)
        {
            std::cout << CYAN + getTimeOfDay() + " [info] : Trying to create server ‘" ITALIC;
            std::cout << serverConfig.at(i).getServerName().at(0) << DEFAULT CYAN "’..." DEFAULT << std::endl;
            _servers.push_back(Server(serverConfig.at(i)));
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

GlobalServer::~GlobalServer() // Close every server fd's here
{
    std::string pad(" ", 4);

    std::cout << MAGENTA BOLD "~GlobalServer():" DEFAULT MAGENTA << std::endl;
    for (std::size_t i = 0; i < this->_servers.size(); i++)
    {
        std::cout << pad << MAGENTA "For server \'" << this->_globalConfig.getServerConfig().at(i).getServerName().at(0) << "\' [" << i << "]:" DEFAULT << std::endl;
        this->_servers.at(i).closeListenSockets();
    }
    std::cout << DEFAULT;
}
