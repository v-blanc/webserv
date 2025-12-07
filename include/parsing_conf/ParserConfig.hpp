/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfigConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:26:57 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/07 14:47:30 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSERCONFIG_HPP
#define PARSERCONFIG_HPP

#include "LexerConfig.hpp"
#include "utils.hpp"

class ParserConfig
{
public:
    ParserConfig(std::ifstream &file, std::string &fileName);
    ~ParserConfig();
    Node parse();

private:
    std::string _fileName;
    LexerConfig _lexer;
    Token _currToken;

    Node parseStatement();
    void nextToken();
};

// TODO: Debug
void printNode(const Node &n, int indent = 0);

#endif
