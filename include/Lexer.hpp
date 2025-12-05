/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 10:44:22 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 12:23:47 by vblanc           ###   ########.fr       */
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
    int col;
};

class Lexer
{
public:
    Lexer(const char *fileName);
    ~Lexer();

private:
    std::string _fileName;
    std::ifstream _file;
    int _line;
    char _c;
    Token _last;

    int openFile(void);
    void closeFile(void);

    void nextChar();
};

#endif
