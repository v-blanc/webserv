/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structConfig.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:29:03 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/07 14:43:20 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTCONFIG_H
#define STRUCTCONFIG_H

#include "define.h"

struct Token
{
    enum Type
    {
        WORD,
        LBRACE,
        RBRACE,
        SEMICOLON,
        END
    } type;

    std::string content;
    int line;
};

struct Node
{
    std::string directive;
    std::vector<std::string> args;
    std::vector<Node> children;
    std::string line;
};

#endif
