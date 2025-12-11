/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isInvalidPath.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 14:20:46 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/11 14:29:40 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

bool isInvalidPath(const std::string &path)
{
    // MacOS/Linux
    if (path.find("..") != std::string::npos || path.find("..") != std::string::npos)
        return (true);
    // Windows
    if (path.find('\\') != std::string::npos || path.find("//") != std::string::npos)
        return (true);
    // Encode '.'
    if (path.find("%2e") != std::string::npos || path.find("%2E") != std::string::npos)
        return (true);
    // Encode '\'
    if (path.find("%2c") != std::string::npos || path.find("%2C") != std::string::npos)
        return (true);
    // Encode '/'
    if (path.find("%2f") != std::string::npos || path.find("%2F") != std::string::npos)
        return (true);
    return (false);
}
