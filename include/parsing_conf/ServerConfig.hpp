/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:35:55 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 18:04:40 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include "configStructs.h"

class ServerConfig
{
public:
    ServerConfig(Node &node, std::string &fileName, int autoindex, long long clientMaxBodySize, std::string root,
                 std::vector<std::string> errorPage);
    ~ServerConfig();

    // Getter
    std::string getFileName() const { return this->_fileName; };
    int getAutoindex() const { return this->_autoindex; };
    long long getClientMaxBodySize() const { return this->_clientMaxBodySize; };
    std::string getRoot() const { return this->_root; };
    std::vector<std::string> getIndex() const { return this->_index; };
    std::vector<std::string> getErrorPage() const { return this->_errorPage; };
    std::vector<std::string> getListen() const { return this->_listen; };
    std::vector<std::string> getServerName() const { return this->_serverName; };
    std::vector<stringPair> getCgiHandler() const { return this->_cgiHandler; };

    // Setter
    void setAutoindex(const int autoindex) { this->_autoindex = autoindex; };
    void setClientMaxBodySize(const long long clientMaxBodySize) { this->_clientMaxBodySize = clientMaxBodySize; };
    void setRoot(const std::string root) { this->_root = root; };
    void pushBackIndex(const std::string index) { this->_index.push_back(index); };
    void pushBackErrorPage(const std::string errorPage) { this->_errorPage.push_back(errorPage); };
    void pushBackListen(const std::string listen) { this->_listen.push_back(listen); };
    void pushBackServerName(const std::string serverName) { this->_serverName.push_back(serverName); };
    void pushBackCgiHandler(const stringPair cgiHandler) { this->_cgiHandler.push_back(cgiHandler); };

private:
    Node &_node;
    std::string &_fileName;

    int _autoindex;
    long long _clientMaxBodySize;
    std::string _root;
    std::vector<std::string> _index;
    std::vector<std::string> _errorPage;
    std::vector<std::string> _listen;     // No limit (but given 1 by 1)
    std::vector<std::string> _serverName; // No limit
    std::vector<stringPair> _cgiHandler;  // 2 (ext and interpreter)
    std::vector<LocationConfig> _locationConfig;

    void handleLocationDirective(Node &node, std::string &directive);
    void fillServerConfig();
};

#endif
