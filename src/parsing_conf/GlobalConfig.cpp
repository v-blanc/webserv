/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:40 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 19:51:16 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"

GlobalConfig::GlobalConfig(const char *fileName) : _fileName(fileName), _file(fileName)
{
    try
    {
        this->safeOpenFile();
        Node root = Parser(this->_file).parse();
        printNode(root);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return;
    }
}

GlobalConfig::~GlobalConfig()
{
    this->safeCloseFile();
}

void GlobalConfig::safeOpenFile()
{
    if (!this->_file.is_open())
    {
        std::string errorMessage = RED "Error with ‘" ITALIC + this->_fileName + DEFAULT RED "’ configuration file: \"" ITALIC;
        if (this->_file.fail())
            errorMessage += strerror(errno);
        errorMessage += "\"" DEFAULT;

        throw std::runtime_error(errorMessage);
    }
    std::cout << GREEN "Configuration file ‘" ITALIC << this->_fileName << DEFAULT GREEN "’ was opened sucessfully!" DEFAULT << std::endl;
}

void GlobalConfig::safeCloseFile()
{
    if (!this->_file.is_open())
        return;
    this->_file.close();
    if (this->_file.fail() && !this->_file.eof())
    {
        std::cerr << RED "Error with ‘" ITALIC << this->_fileName << DEFAULT RED "’ configuration file: \"" ITALIC;
        std::cerr << "The file couldn't be closed properly\"" DEFAULT << std::endl;
        return;
    }
    std::cout << GREEN "Configuration file ‘" ITALIC << this->_fileName << DEFAULT GREEN "’ was closed sucessfully!" DEFAULT << std::endl;
}

