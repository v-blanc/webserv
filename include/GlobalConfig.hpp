/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:30:16 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 18:24:02 by vblanc           ###   ########.fr       */
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
};

#endif
