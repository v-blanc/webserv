/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:30:16 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/09 18:16:24 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "ParserConfig.hpp"
#include "ServerConfig.hpp"
#include "utils.hpp"

class GlobalConfig
{
public:
    GlobalConfig(const char *fileName);
    ~GlobalConfig() {};

    // Getter
    std::string getFileName() const { return this->_fileName; };
    int getAutoindex() const { return this->_autoindex; };
    long long getClientMaxBodySize() const { return this->_clientMaxBodySize; };
    std::string getRoot() const { return this->_root; };
    std::vector<std::string> getIndex() const { return this->_index; };
    std::map<std::size_t, std::string> getErrorPage() const { return this->_errorPage; };
    std::vector<ServerConfig> getServerConfig() const { return this->_serverConfig; };

    // Setter
    void setAutoindex(const int autoindex) { this->_autoindex = autoindex; };
    void setClientMaxBodySize(const long long clientMaxBodySize) { this->_clientMaxBodySize = clientMaxBodySize; };
    void setRoot(const std::string root) { this->_root = root; };
    void pushBackIndex(const std::string index) { this->_index.push_back(index); };
    void pushBackErrorPage(const std::size_t errorCode, const std::string file) { this->_errorPage[errorCode] = file; };

private:
    // Parsing attributes
    std::string _fileName;
    std::ifstream _file;
    Node _rootNode;

    // Config attributes
    int _autoindex;
    long long _clientMaxBodySize;
    std::string _root;
    std::vector<std::string> _index;
    std::map<std::size_t, std::string> _errorPage;
    std::vector<ServerConfig> _serverConfig;

    // File Management
    void safeOpenFile();
    void safeCloseFile();

    // Fill Global Config
    void handleServerDirective(Node &node, std::string &directive);
    void fillGlobalConfig();
};

void printGlobalConfig(GlobalConfig &globalConfig);

#endif
