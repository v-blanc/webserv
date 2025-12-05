/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:30:16 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 19:49:48 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "Parser.hpp"

class GlobalConfig
{
public:
    GlobalConfig(const char *fileName);
    ~GlobalConfig();

private:
    std::string _fileName;
    std::ifstream _file;
    Node _root;

    // File Management
    void safeOpenFile();
    void safeCloseFile();
};

#endif
