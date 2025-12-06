/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 19:38:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 17:48:22 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig(Node &node, std::string &fileName, int autoindex, long long clientMaxBodySize,
                               std::string root, std::vector<std::string> index, std::vector<std::string> errorPage,
                               std::vector<stringPair> cgiHandler)
    : _node(node), _fileName(fileName)
{
    // Inherited values
    this->_autoindex = autoindex;
    this->_clientMaxBodySize = clientMaxBodySize;
    this->_root = root;
    this->_index = index;
    this->_errorPage = errorPage;
    this->_cgiHandler = cgiHandler;
    (void)this->_node;
    (void)this->_fileName;
}

LocationConfig::~LocationConfig()
{
}
