/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 17:26:57 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 17:48:00 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "utils.hpp"

class Parser
{
public:
    Parser(std::ifstream &file, std::string &fileName);
    ~Parser();
    Node parse();

private:
    std::string _fileName;
    Lexer _lexer;
    Token _currToken;

    Node parseStatement();
    void nextToken();
};

// TODO: Debug
void printNode(const Node &n, int indent = 0);

#endif
