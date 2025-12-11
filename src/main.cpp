/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/11 18:59:30 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalServer.hpp"

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cerr << RED "Usage: ./webser [configuration]" DEFAULT << std::endl;
        return (ERROR);
    }

    std::string configFile = DEFAULT_CONFIG_PATH;
    if (argc == 2)
        configFile = argv[1];

    GlobalConfig globalConfig(configFile.c_str());
    // printGlobalConfig(globalConfig);

    GlobalServer gloablServer(globalConfig);

    return (SUCCESS);
}
