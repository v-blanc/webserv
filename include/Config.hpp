/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:30:16 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/04 18:03:06 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
#define CONFIG_H

#include "colors.h"
#include "define.h"
#include <cstring>
#include <fstream>
#include <iostream>

// Attributes and methods can be renamed or removed, and params can be changed

class Config
{
public:
    Config(const char *fileName);
    ~Config();

private:
    std::string _fileName;
    std::ifstream _file;

    int openFile(void);
    void closeFile(void);
};

#endif
