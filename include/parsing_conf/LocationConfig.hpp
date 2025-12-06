/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:37:24 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 17:48:02 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "configStructs.h"
#include "utils.hpp"

class LocationConfig
{
public:
    LocationConfig(Node &node, std::string &fileName, int autoindex, long long clientMaxBodySize, std::string root,
                   std::vector<std::string> index, std::vector<std::string> errorPage,
                   std::vector<stringPair> cgiHandler);
    ~LocationConfig();

private:
    Node &_node;
    std::string &_fileName;

    int _autoindex;
    long long _clientMaxBodySize;
    std::string _root;
    std::string _uploadStore;
    std::vector<std::string> _index;       // No limit
    std::vector<std::string> _errorPage;   // No limit (at least 2, last URI)
    std::vector<std::string> _limitExcept; // No limit
    std::vector<std::string> _return;      // 1 or 2 (code URL)
    std::vector<stringPair> _cgiHandler;   // 2 (ext and interpreter)
};

#include "directiveHandler.tpp"

#endif
