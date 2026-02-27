/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:37:24 by vblanc            #+#    #+#             */
/*   Updated: 2026/02/27 17:53:04 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "structConfig.h"
#include "utils.hpp"

class LocationConfig
{
public:
    LocationConfig(Node &node, std::string &path, std::string &fileName, int autoindex, long long clientMaxBodySize,
                   std::string root, std::vector<std::string> index, std::map<std::size_t, std::string> errorPage,
                   std::vector<stringPair> cgiHandler);
    LocationConfig(void);
    LocationConfig& operator=(const LocationConfig &other);
    ~LocationConfig();

    // Getter
    std::string getFileName() const { return this->_fileName; };
    std::string getPath() const { return this->_path; };
    int getAutoindex() const { return this->_autoindex; };
    long long getClientMaxBodySize() const { return this->_clientMaxBodySize; };
    std::string getRoot() const { return this->_root; };
    std::string getUploadStore() const { return this->_uploadStore; };
    std::string getReturn() const { return this->_return; };
    std::vector<std::string> getIndex() const { return this->_index; };
    std::map<std::size_t, std::string> getErrorPage() const { return this->_errorPage; };
    std::vector<std::string> getLimitExcept() const { return this->_limitExcept; };
    std::vector<stringPair> getCgiHandler() const { return this->_cgiHandler; };

    // Setter
    void setAutoindex(const int autoindex) { this->_autoindex = autoindex; };
    void setClientMaxBodySize(const long long clientMaxBodySize) { this->_clientMaxBodySize = clientMaxBodySize; };
    void setRoot(const std::string root) { this->_root = root; };
    void setUploadStore(const std::string uploadStore) { this->_uploadStore = uploadStore; };
    void setReturn(const std::string returnStr) { this->_return = returnStr; };
    void setIndex(std::vector<std::string> index){ this->_index = index;};
    void pushBackIndex(const std::string index) { this->_index.push_back(index); };
    void pushBackErrorPage(const std::size_t errorCode, const std::string file) { this->_errorPage[errorCode] = file; };
    void pushBackLimitExcept(const std::string limitExcept) { this->_limitExcept.push_back(limitExcept); };
    void pushBackCgiHandler(const stringPair cgiHandler) { this->_cgiHandler.push_back(cgiHandler); };

private:
    Node &_node;
    std::string &_fileName;

    std::string _path;
    int _autoindex;
    long long _clientMaxBodySize;
    std::string _root;
    std::string _uploadStore;
    std::string _return;                            // 1 or 2 (code URL)
    std::vector<std::string> _index;               // No limit
    std::map<std::size_t, std::string> _errorPage; // No limit (at least 2, last URI)
    std::vector<std::string> _limitExcept;         // No limit
    std::vector<stringPair> _cgiHandler;           // 2 (ext and interpreter)

    void fillLocationConfig();
};

#include "directiveHandlerConfig.tpp"

#endif
