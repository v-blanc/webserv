/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:12:25 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 15:18:42 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALSERVER_HPP
#define GLOBALSERVER_HPP

#include "Server.hpp"

class GlobalServer
{
public:
    GlobalServer(GlobalConfig &globalConfig);
    ~GlobalServer();

private:
    GlobalConfig &_globalConfig;
    std::vector<Server> _servers;
};

#endif
