/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:40 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 15:57:13 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"

GlobalConfig::GlobalConfig(const char *fileName) : _fileName(fileName), _file(fileName), _autoindex(-1), _clientMaxBodySize(-1)
{
    try
    {
        this->safeOpenFile();
        this->_rootNode = Parser(this->_file, this->_fileName).parse();
        printNode(this->_rootNode);
        std::cout << std::endl;
        this->fillGlobalConfig();
        this->printGlobalConfig();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error: " << e.what() << "\n";
    //     return;
    // }
}

GlobalConfig::~GlobalConfig()
{
    this->safeCloseFile();
}

void GlobalConfig::safeOpenFile()
{
    if (!this->_file.is_open())
        throwSafeOpenFileError(this->_file, this->_fileName);
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

void GlobalConfig::handleServerDirective(Node &node, std::string &directive)
{
    if (node.args.size() != 0)
        throwInvalidNumberOfArguments(directive, this->_fileName, node.line);
    else
        this->_serverConfig.push_back(ServerConfig(
            this->_autoindex, this->_clientMaxBodySize, this->_root, this->_index, this->_errorPage));
}

void GlobalConfig::fillGlobalConfig()
{
    for (std::size_t i = 0; i < this->_rootNode.children.size(); i++)
    {
        Node currNode = this->_rootNode.children.at(i);
        std::string directive = this->_rootNode.children.at(i).directive;

        if (directive == "server")
            handleServerDirective(currNode, directive);
        else if (directive == "root")
            handleRootDirective(*this, currNode, directive);
        else if (directive == "index")
            handleIndexDirective(*this, currNode);
        else if (directive == "error_page")
            handleErrorPageDirective(*this, currNode, directive);
        else if (directive == "autoindex")
            handleAutoindex(*this, currNode, directive);
        else if (directive == "client_max_body_size")
            handleClientMaxBodySize(*this, currNode, directive);
        else
            throwUnknownDirective(directive, this->_fileName, currNode.line);
    }

    // Default values
    if (this->_root.empty())
        this->_root = "html";

    if (this->_index.size() == 0)
        this->_index.push_back("index.html");

    if (this->_autoindex == -1)
        this->_autoindex = false;

    if (this->_clientMaxBodySize == -1)
        this->_clientMaxBodySize = 1e6; // 1M
}

void GlobalConfig::printGlobalConfig()
{
    std::string pad(4, ' ');
    std::cout << "Global Config:" << std::endl;

    std::cout << pad << "root: " << this->_root << std::endl;

    std::cout << pad << "index: ";
    for (std::size_t i = 0; i < this->_index.size(); i++)
        std::cout << this->_index[i] << ", ";
    std::cout << std::endl;

    std::cout << pad << "error_page: ";
    for (std::size_t i = 0; i < this->_errorPage.size(); i++)
        std::cout << this->_errorPage[i] << ", ";
    std::cout << std::endl;

    std::cout << pad << "autoindex: " << (this->_autoindex == true ? "on" : "off") << std::endl;

    std::cout << pad << "client_max_body_size: " << this->_clientMaxBodySize << std::endl;
}
