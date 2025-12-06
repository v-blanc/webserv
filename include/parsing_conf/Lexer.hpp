/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 10:44:22 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 17:48:04 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
#define LEXER_HPP

#include "colors.h"
#include "define.h"
#include "configStructs.h"

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
