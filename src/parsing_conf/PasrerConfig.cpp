/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PasrerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:26:32 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/07 14:48:10 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserConfig.hpp"

ParserConfig::ParserConfig(std::ifstream &file, std::string &fileName) : _fileName(fileName), _lexer(file)
{
    this->_currToken = this->_lexer.nextToken();
}

ParserConfig::~ParserConfig()
{
}

Node ParserConfig::parse()
{
    Node root;
    root.directive = "";
    root.line = "1";

    while (this->_currToken.type != Token::END)
        root.children.push_back(this->parseStatement());

    return root;
}

Node ParserConfig::parseStatement()
{
    if (this->_currToken.type != Token::WORD)
        throwUnexpectedType(this->_currToken.type, this->_currToken.line, this->_fileName);

    Node node;
    node.directive = this->_currToken.content;
    std::stringstream ss;
    ss << this->_currToken.line;
    ss >> node.line;

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
            throwUnexpectedEOF(this->_currToken.line, this->_fileName);
        this->nextToken();
        break;
    default:
        throwUnexpectedType(this->_currToken.type, this->_currToken.line, this->_fileName);
        break;
    }

    return node;
}

void ParserConfig::nextToken()
{
    this->_currToken = this->_lexer.nextToken();
}

void printNode(const Node &n, int indent)
{
    std::string pad(indent * 4, ' ');

    if (!n.directive.empty())
    {
        std::cout << pad << "(dir):" << n.directive;
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
