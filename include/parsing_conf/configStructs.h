/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configStructs.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:29:03 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 17:48:10 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSTRUCTS_H
#define CONFIGSTRUCTS_H

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
