/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:26:57 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 18:23:25 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include <vector>

struct Node
{
    std::string name;
    std::vector<std::string> args;
    std::vector<Node> children;
    int line;
};

class Parser
{
public:
    Parser(std::ifstream &_file);
    ~Parser();
    Node parse();

private:
    Lexer _lexer;
    Token _currToken;

    Node parseStatement();
    void nextToken();
};

// TODO: Debug
void printNode(const Node &n, int indent = 0);

#endif
