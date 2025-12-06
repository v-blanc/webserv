/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:39:00 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 19:31:16 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig(Node &node, std::string &fileName, int autoindex, long long clientMaxBodySize,
                           std::string root, std::vector<std::string> errorPage)
    : _node(node), _fileName(fileName), _autoindex(-1), _clientMaxBodySize(-1)
{
    this->fillServerConfig();

    // Inherited values
    if (this->_autoindex == -1)
        this->_autoindex = autoindex;
    if (this->_clientMaxBodySize == -1)
        this->_clientMaxBodySize = clientMaxBodySize;
    if (this->_root.empty())
        this->_root = root;
    if (this->_errorPage.size() == 0)
        this->_errorPage = errorPage;

    // Handle location direction
    for (std::size_t i = 0; i < this->_node.children.size(); i++)
    {
        Node currNode = this->_node.children.at(i);
        std::string directive = this->_node.children.at(i).directive;

        if (directive == "location")
            handleLocationDirective(currNode, directive);
    }
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::handleLocationDirective(Node &node, std::string &directive)
{
    if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, this->_fileName, node.line);
    else
        this->_locationConfig.push_back(LocationConfig(
            node, node.args.at(0), this->_fileName, this->_autoindex, this->_clientMaxBodySize, this->_root, this->_index, this->_errorPage, this->_cgiHandler));
}

void ServerConfig::fillServerConfig()
{
    for (std::size_t i = 0; i < this->_node.children.size(); i++)
    {
        Node currNode = this->_node.children.at(i);
        std::string directive = this->_node.children.at(i).directive;

        if (directive == "location")
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
        else if (directive == "listen")
            handleListenDirective(*this, currNode, directive);
        else if (directive == "server_name")
            handleServerNameDirective(*this, currNode, directive);
        else if (directive == "cgi_handler")
            handleCgiHandlerDirective(*this, currNode, directive);
        else
            throwUnknownDirective(directive, this->_fileName, currNode.line);
    }

    // Default values
    if (this->_index.size() == 0)
        this->_index.push_back("index.html");
    if (this->_listen.size() == 0)
        this->_listen.push_back("DEFAULT VALUE TO SET"); // TODO
    if (this->_serverName.size() == 0)
        this->_serverName.push_back("");
}
