/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 12:00:57 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 19:51:14 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

Lexer::Lexer(std::ifstream &file) : _file(file), _line(1), _pushed(false)
{
    this->nextChar();
}

Lexer::~Lexer()
{
}

static int isSpecialChar(char c)
{
    return (c == '#' || c == '{' || c == '}' || c == ';' || c == EOF);
}

Token Lexer::nextToken()
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

Token Lexer::currToken()
{
    Token t = this->nextToken();
    this->_pushed = true;
    return t;
}

void Lexer::nextChar()
{
    this->_c = this->_file.get();
    if (this->_c == '\n')
        this->_line++;
    if (this->_file.eof())
        this->_c = EOF;
}

void Lexer::skipWhiteSpaceAndComments()
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

static const char *tokenName(Token::Type t)
{
    switch (t)
    {
    case Token::WORD:
        return "WORD";
    case Token::LBRACE:
        return "LBRACE";
    case Token::RBRACE:
        return "RBRACE";
    case Token::SEMICOLON:
        return "SEMICOLON";
    case Token::END:
        return "END";
    default:
        return "UNKNOWN";
    }
}

void printLexer(Lexer lexer)
{
    for (;;)
    {
        Token t = lexer.currToken();
        std::cout << tokenName(t.type) << " (line " << t.line << ")";
        if (t.type == Token::WORD)
            std::cout << " : [" << t.content << "]";
        std::cout << "\n";
        if (t.type == Token::END)
            break;
        lexer.nextToken();
    }
}
