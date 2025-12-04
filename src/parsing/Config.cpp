/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:40 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/04 13:03:08 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(const char *fileName) : _fileName(fileName), _file(fileName)
{
    this->openFile();
}

Config::~Config()
{
    this->closeFile();
}

int Config::openFile(void)
{
    if (!this->_file.is_open())
    {
        std::cerr << RED "Error with ‘" ITALIC << this->_fileName << DEFAULT RED "’ configuration file: \"" ITALIC;
        if (this->_file.fail())
            std::cerr << strerror(errno);
        std::cerr << "\"" DEFAULT << std::endl;
        return (ERROR);
    }
    std::cout << GREEN "Configuration file ‘" ITALIC << this->_fileName << DEFAULT GREEN "’ was opened sucessfully!" DEFAULT << std::endl;
    return (SUCCESS);
}

void Config::closeFile(void)
{
    this->_file.close();
}
