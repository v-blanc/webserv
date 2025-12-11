/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorMessageServer.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 17:38:16 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 17:53:51 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void handleBindError(std::string listen, std::string fileName)
{
    std::string errorMessage = YELLOW + getTimeOfDay() + " [warning] : host not found in \"" + listen;
    errorMessage += +"\" of the \"listen\" directive in " + fileName + DEFAULT;

    std::cerr << errorMessage << std::endl;
}

void throwMajorIssueCreatingServer(std::string serverName)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : Major issue encountered during creation of server ‘";
    errorMessage += ITALIC + serverName + DEFAULT RED "’" DEFAULT;

    throw std::runtime_error(errorMessage);
}
