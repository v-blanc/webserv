/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:31:40 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/11 12:51:20 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"

GlobalConfig::GlobalConfig(const char *fileName) : _fileName(fileName), _file(fileName), _autoindex(-1), _clientMaxBodySize(-1)
{
    try
    {
        this->safeOpenFile();
        this->_rootNode = ParserConfig(this->_file, this->_fileName).parse();
        // printNode(this->_rootNode);
        // std::cout << std::endl;

        this->fillGlobalConfig();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        this->safeCloseFile();
        std::exit(ERROR);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected Error: " << e.what() << std::endl;
        this->safeCloseFile();
        std::exit(ERROR);
    }

    this->safeCloseFile();
}

void GlobalConfig::safeOpenFile()
{
    if (!this->_file.is_open())
        throwSafeOpenFileError(this->_file, this->_fileName);
    std::cout << GREEN + getTimeOfDay() + " [ok] : Configuration file ‘" ITALIC << this->_fileName << DEFAULT;
    std::cout << GREEN "’ was opened sucessfully!" DEFAULT << std::endl;
}

void GlobalConfig::safeCloseFile()
{
    if (!this->_file.is_open())
        return;
    this->_file.close();
    if (this->_file.fail() && !this->_file.eof())
    {
        std::cerr << RED + getTimeOfDay() + " [warning] : Error with ‘" ITALIC << this->_fileName << DEFAULT RED "’ configuration file: \"" ITALIC;
        std::cerr << "The file couldn't be closed properly\"" DEFAULT << std::endl;
        return;
    }
    std::cout << GREEN + getTimeOfDay() + " [ok] : Configuration file ‘" ITALIC << this->_fileName << DEFAULT;
    std::cout << GREEN "’ was closed sucessfully!" DEFAULT << std::endl;
}

void GlobalConfig::handleServerDirective(Node &node, std::string &directive)
{
    if (node.args.size() != 0)
        throwInvalidNumberOfArguments(directive, this->_fileName, node.line);
    else
        this->_serverConfig.push_back(ServerConfig(
            node, this->_fileName, this->_autoindex, this->_clientMaxBodySize, this->_root, this->_errorPage));
}

void GlobalConfig::fillGlobalConfig()
{
    for (std::size_t i = 0; i < this->_rootNode.children.size(); i++)
    {
        Node currNode = this->_rootNode.children.at(i);
        std::string directive = this->_rootNode.children.at(i).directive;

        if (directive == "server")
            continue;
        else if (directive == "autoindex")
            handleAutoindex(*this, currNode, directive);
        else if (directive == "client_max_body_size")
            handleClientMaxBodySize(*this, currNode, directive);
        else if (directive == "root")
            handleRootDirective(*this, currNode, directive);
        else if (directive == "index")
            handleIndexDirective(*this, currNode);
        else if (directive == "error_page")
            handleErrorPageDirective(*this, currNode, directive);
        else
            throwUnknownDirective(directive, this->_fileName, currNode.line);
    }

    // Default values
    if (this->_autoindex == -1)
        this->_autoindex = false;
    if (this->_clientMaxBodySize == -1)
        this->_clientMaxBodySize = 1e6;
    if (this->_root.empty())
        this->_root = "html";
    if (this->_index.size() == 0)
        this->_index.push_back("index.html");

    for (std::size_t i = 0; i < this->_rootNode.children.size(); i++)
    {
        Node currNode = this->_rootNode.children.at(i);
        std::string directive = this->_rootNode.children.at(i).directive;

        if (directive == "server")
            handleServerDirective(currNode, directive);
    }

    // Check duplicates listen among servers
    std::set<listenPair> seen;

    for (std::size_t i = 0; i < this->_serverConfig.size(); i++)
    {
        std::vector<listenPair> listen = this->_serverConfig.at(i).getListen();

        for (size_t j = 0; j < listen.size(); j++)
        {
            if (seen.count(listen.at(j)))
                throwDuplicateValues("listen", this->_serverConfig.at(i).getListenStr().at(j), this->_fileName, "");
            seen.insert(listen.at(j));
        }
    }
}

void printGlobalConfig(GlobalConfig &globalConfig)
{
    int indent = 4;
    std::string pad(0, ' ');
    std::cout << "******** Global Config: ********" << std::endl;

    std::cout << pad << "autoindex: " << (globalConfig.getAutoindex() == true ? "on" : "off") << std::endl;
    std::cout << pad << "client_max_body_size: " << globalConfig.getClientMaxBodySize() << std::endl;
    std::cout << pad << "root: " << globalConfig.getRoot() << std::endl;
    std::cout << pad << "index: ";
    for (std::size_t i = 0; i < globalConfig.getIndex().size(); i++)
        std::cout << globalConfig.getIndex()[i] << ", ";
    std::cout << std::endl;
    std::cout << pad << "error_page: ";
    for (std::size_t i = 0; i < globalConfig.getErrorPage().size(); i++)
        std::cout << globalConfig.getErrorPage()[i] << ", ";
    std::cout << std::endl;
    std::cout << std::endl;

    std::vector<ServerConfig> serverConfig = globalConfig.getServerConfig();
    for (std::size_t j = 0; j < serverConfig.size(); j++)
    {
        std::string pad(indent, ' ');
        std::cout << "Server Config:" << std::endl;

        std::cout << pad << "autoindex: " << (serverConfig.at(j).getAutoindex() == true ? "on" : "off") << std::endl;
        std::cout << pad << "client_max_body_size: " << serverConfig.at(j).getClientMaxBodySize() << std::endl;
        std::cout << pad << "root: " << serverConfig.at(j).getRoot() << std::endl;
        std::cout << pad << "index: ";
        for (std::size_t i = 0; i < serverConfig.at(j).getIndex().size(); i++)
            std::cout << serverConfig.at(j).getIndex().at(i) << ", ";
        std::cout << std::endl;
        std::cout << pad << "error_page: ";
        for (std::size_t i = 0; i < serverConfig.at(j).getErrorPage().size(); i++)
            std::cout << serverConfig.at(j).getErrorPage().at(i) << ", ";
        std::cout << std::endl;
        std::cout << pad << "listen: " << std::endl;
        for (std::size_t i = 0; i < serverConfig.at(j).getListen().size(); i++)
        {
            std::cout << pad << pad << "[" << i << "] host: \'" << serverConfig.at(j).getListen().at(i).first;
            std::cout << "\' port: \'" << serverConfig.at(j).getListen().at(i).second << "\'" << std::endl;
        }
        std::cout << pad << "server_name: ";
        for (std::size_t i = 0; i < serverConfig.at(j).getServerName().size(); i++)
            std::cout << serverConfig.at(j).getServerName().at(i) << ", ";
        std::cout << std::endl;
        std::cout << pad << "cgi_handler: ";
        for (std::size_t i = 0; i < serverConfig.at(j).getCgiHandler().size(); i++)
        {
            std::cout << "[" << serverConfig.at(j).getCgiHandler().at(i).first << ", ";
            std::cout << serverConfig.at(j).getCgiHandler().at(i).second << "], ";
        }
        std::cout << std::endl;
        std::cout << std::endl;

        std::vector<LocationConfig> locationConfig = serverConfig.at(j).getLocationConfig();
        for (std::size_t k = 0; k < locationConfig.size(); k++)
        {
            std::string pad(2 * indent, ' ');
            std::cout << "    Location Config:" << std::endl;

            std::cout << pad << "path: " << locationConfig.at(k).getPath() << std::endl;
            std::cout << pad << "autoindex: " << (locationConfig.at(k).getAutoindex() == true ? "on" : "off") << std::endl;
            std::cout << pad << "client_max_body_size: " << locationConfig.at(k).getClientMaxBodySize() << std::endl;
            std::cout << pad << "root: " << locationConfig.at(k).getRoot() << std::endl;
            std::cout << pad << "return: [" << locationConfig.at(k).getReturn().first << ", " << locationConfig.at(k).getReturn().second << "]" << std::endl;

            std::cout << pad << "index: ";
            for (std::size_t i = 0; i < locationConfig.at(k).getIndex().size(); i++)
                std::cout << locationConfig.at(k).getIndex().at(i) << ", ";
            std::cout << std::endl;
            std::cout << pad << "error_page: ";
            for (std::size_t i = 0; i < locationConfig.at(k).getErrorPage().size(); i++)
                std::cout << locationConfig.at(k).getErrorPage().at(i) << ", ";
            std::cout << std::endl;
            std::cout << pad << "cgi_handler: ";
            for (std::size_t i = 0; i < locationConfig.at(k).getCgiHandler().size(); i++)
            {
                std::cout << "[" << locationConfig.at(k).getCgiHandler().at(i).first << ", ";
                std::cout << locationConfig.at(k).getCgiHandler().at(i).second << "], ";
            }
            std::cout << std::endl;
            std::cout << pad << "limit_except: ";
            for (std::size_t i = 0; i < locationConfig.at(k).getLimitExcept().size(); i++)
                std::cout << locationConfig.at(k).getLimitExcept().at(i) << ", ";
            std::cout << std::endl;
            std::cout << pad << "upload_store: ";
            for (std::size_t i = 0; i < locationConfig.at(k).getUploadStore().size(); i++)
                std::cout << locationConfig.at(k).getUploadStore().at(i) << ", ";
            std::cout << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
