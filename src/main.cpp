/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/17 15:36:11 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cerr << RED "Usage: ./webserv [configuration]" DEFAULT << std::endl;
        return (ERROR);
    }

    std::string configFile = DEFAULT_CONFIG_PATH;
    if (argc == 2)
        configFile = argv[1];

    // std::string path = "www/";
    // std::cout << generateAutoindexHTML(path) << std::endl;

    GlobalConfig globalConfig(configFile.c_str());
    printGlobalConfig(globalConfig);

    GlobalServer globalServer(globalConfig);

    return (SUCCESS);
}
