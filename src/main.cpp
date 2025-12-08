/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 15:23:22 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"
#include "HTTPRequest.hpp"
#include "GlobalServer.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Usage: ./webser configuration" DEFAULT << std::endl;
        return (ERROR);
    }

    (void)argv;
    GlobalConfig globalConfig(argv[1]);
    // printGlobalConfig(globalConfig);

    GlobalServer gloablServer(globalConfig);

    // HTTPRequest httpRequest(request);
    // printHTTPRequest(httpRequest);

    return (SUCCESS);
}
