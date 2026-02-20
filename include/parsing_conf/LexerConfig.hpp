/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 10:44:22 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/20 16:07:52 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXERCONFIG_HPP
#define LEXERCONFIG_HPP

#include "colors.h"
#include "define.h"
#include "structConfig.h"

class LexerConfig
{
public:
    LexerConfig(std::ifstream &_file);
    ~LexerConfig();

    Token nextToken();
    Token currToken();

private:
    std::ifstream &_file;

    int _c;
    int _line;
    Token _lastToken;
    bool _pushed;

    void nextChar();
    void skipWhiteSpaceAndComments();
};

// TODO: Debug
void printLexerConfig(LexerConfig lexer);

#endif
