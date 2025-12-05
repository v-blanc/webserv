/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pasrer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:26:32 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 18:47:02 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(std::ifstream &_file) : _lexer(_file)
{
    this->_currToken = this->_lexer.nextToken();
}

Parser::~Parser()
{
}

Node Parser::parse()
{
    Node root;
    root.name = "";
    root.line = 1;

    while (this->_currToken.type != Token::END)
        root.children.push_back(this->parseStatement());

    return root;
}

Node Parser::parseStatement()
{
    if (this->_currToken.type != Token::WORD)
        throw std::runtime_error("unexpected \"*type*\" in ...");

    Node node;
    node.name = this->_currToken.content;
    node.line = this->_currToken.line;

    this->nextToken();

    while (this->_currToken.type == Token::WORD)
    {
        node.args.push_back(this->_currToken.content);
        this->nextToken();
    }

    switch (this->_currToken.type)
    {
    case Token::SEMICOLON:
        this->nextToken();
        break;
    case Token::LBRACE:
        this->nextToken();

        while (this->_currToken.type != Token::RBRACE && this->_currToken.type != Token::END)
            node.children.push_back(parseStatement());

        if (this->_currToken.type != Token::RBRACE)
            throw std::runtime_error("unexpected end of file, expecting \"}\" in ...");
        this->nextToken();
        break;
    default:
        throw std::runtime_error("unexpected \"*type*\" in ...");
        break;
    }

    return node;
}

void Parser::nextToken()
{
    this->_currToken = this->_lexer.nextToken();
}

void printNode(const Node &n, int indent)
{
    std::string pad(indent * 4, ' ');

    if (!n.name.empty())
    {
        std::cout << pad << "(dir):" << n.name;
        std::cout << " (args):";
        for (size_t i = 0; i < n.args.size(); ++i)
            std::cout << " \"" << n.args[i] << "\"";
        if (n.children.empty())
            std::cout << " ;\n";
        else
        {
            std::cout << " {\n";
            for (size_t i = 0; i < n.children.size(); ++i)
                printNode(n.children[i], indent + 1);
            std::cout << pad << "}\n";
        }
    }
    else // root
        for (size_t i = 0; i < n.children.size(); ++i)
            printNode(n.children[i], indent);
}
