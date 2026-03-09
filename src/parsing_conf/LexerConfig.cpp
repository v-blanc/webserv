/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 12:00:57 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/09 18:37:05 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LexerConfig.hpp"

LexerConfig::LexerConfig(std::ifstream &file) : _file(file), _line(1), _pushed(false)
{
    this->nextChar();
}

LexerConfig::~LexerConfig()
{
}

static int isSpecialChar(int c)
{
    return (c == '#' || c == '{' || c == '}' || c == ';' || c == EOF);
}

Token LexerConfig::nextToken()
{
    if (this->_pushed)
    {
        this->_pushed = false;
        return this->_lastToken;
    }

    skipWhiteSpaceAndComments();

    Token t;
    t.line = this->_line;

    switch (this->_c)
    {
    case '{':
        t.type = Token::LBRACE;
        t.content = "{";
        this->nextChar();
        break;
    case '}':
        t.type = Token::RBRACE;
        t.content = "}";
        this->nextChar();
        break;
    case ';':
        t.type = Token::SEMICOLON;
        t.content = ";";
        this->nextChar();
        break;
    case EOF:
        t.type = Token::END;
        t.content = "";
        break;
    default:
        t.type = Token::WORD;
        while (!isSpecialChar(this->_c) && !isspace(this->_c))
        {
            t.content.push_back(this->_c);
            this->nextChar();
        }

        if (t.content.empty())
            return this->nextToken();
        break;
    }

    this->_lastToken = t;
    return t;
}

Token LexerConfig::currToken()
{
    Token t = this->nextToken();
    this->_pushed = true;
    return t;
}

void LexerConfig::nextChar()
{
    this->_c = this->_file.get();
    if (this->_c == '\n')
        this->_line++;
    if (this->_file.eof())
        this->_c = EOF;
}

void LexerConfig::skipWhiteSpaceAndComments()
{
    while (true)
    {
        while (this->_c != EOF && isspace(this->_c))
            this->nextChar();
        if (this->_c == '#')
        {
            while (this->_c != EOF && this->_c != '\n')
                this->nextChar();
            continue;
        }
        break;
    }
}
