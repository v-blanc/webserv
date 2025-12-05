/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 10:44:22 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 18:23:32 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include "colors.h"
#include "define.h"
#include <cstring>
#include <iostream>
#include <fstream>

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

class Lexer
{
public:
    Lexer(std::ifstream &_file);
    ~Lexer();

    Token nextToken();
    Token currToken();

private:
    std::ifstream &_file;

    char _c;
    int _line;
    Token _lastToken;
    bool _pushed;

    void nextChar();
    void skipWhiteSpaceAndComments();
};

// TODO: Debug
void printLexer(Lexer lexer);

#endif
