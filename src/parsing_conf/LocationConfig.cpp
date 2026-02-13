/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:38:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/17 19:16:12 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig(Node &node, std::string &path, std::string &fileName, int autoindex, long long clientMaxBodySize,
                               std::string root, std::vector<std::string> index, std::map<std::size_t, std::string> errorPage,
                               std::vector<stringPair> cgiHandler)
    : _node(node), _fileName(fileName), _path(path), _autoindex(-1), _clientMaxBodySize(-1)
{
    this->fillLocationConfig();

    // Inherited values
    if (this->_autoindex == -1)
        this->_autoindex = autoindex;
    if (this->_clientMaxBodySize == -1)
        this->_clientMaxBodySize = clientMaxBodySize;
    if (this->_root.empty())
        this->_root = root;
    if (this->_errorPage.size() == 0)
        this->_errorPage = errorPage;
    if (this->_index.size() == 0)
        this->_index = index;
    if (this->_cgiHandler.size() == 0)
        this->_cgiHandler = cgiHandler;
}

LocationConfig::LocationConfig(void) : _node(*(new Node())), _fileName(*(new std::string())), _path(""), _autoindex(-1), _clientMaxBodySize(-1)
{}

LocationConfig& LocationConfig::operator=(const LocationConfig &other)
{
    if (this != &other)
    {
        this->_path = other._path;
        this->_autoindex = other._autoindex;
        this->_clientMaxBodySize = other._clientMaxBodySize;
        this->_root = other._root;
        this->_uploadStore = other._uploadStore;
        this->_return = other._return;
        this->_index = other._index;
        this->_errorPage = other._errorPage;
        this->_limitExcept = other._limitExcept;
        this->_cgiHandler = other._cgiHandler;
    }
    return (*this);
}

LocationConfig::~LocationConfig()
{
}

void LocationConfig::fillLocationConfig()
{
    for (std::size_t i = 0; i < this->_node.children.size(); i++)
    {
        Node currNode = this->_node.children.at(i);
        std::string directive = this->_node.children.at(i).directive;

        if (directive == "autoindex")
            handleAutoindex(*this, currNode, directive);
        else if (directive == "client_max_body_size")
            handleClientMaxBodySize(*this, currNode, directive);
        else if (directive == "root")
            handleRootDirective(*this, currNode, directive);
        else if (directive == "upload_store")
            handleUpdloadStoreDirective(*this, currNode, directive);
        else if (directive == "return")
            handleReturnDirective(*this, currNode, directive);
        else if (directive == "index")
            handleIndexDirective(*this, currNode);
        else if (directive == "error_page")
            handleErrorPageDirective(*this, currNode, directive);
        else if (directive == "limit_except")
            handleLimitExceptDirective(*this, currNode, directive);
        else if (directive == "cgi_handler")
            handleCgiHandlerDirective(*this, currNode, directive);
        else
            throwUnknownDirective(directive, this->_fileName, currNode.line);
    }

    if (this->_limitExcept.size() == 0)
    {
        this->_limitExcept.push_back("GET");
        this->_limitExcept.push_back("POST");
    }
}
