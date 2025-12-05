/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 17:13:05 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int openFile(std::ifstream &file, std::string fileName)
{
    if (!file.is_open())
    {
        std::cerr << RED "Error with ‘" ITALIC << fileName << DEFAULT RED "’ configuration file: \"" ITALIC;
        if (file.fail())
            std::cerr << strerror(errno);
        std::cerr << "\"" DEFAULT << std::endl;
        return (ERROR);
    }
    std::cout << GREEN "Configuration file ‘" ITALIC << fileName << DEFAULT GREEN "’ was opened sucessfully!" DEFAULT << std::endl;
    return (SUCCESS);
}

void closeFile(std::ifstream &file)
{
    file.close();
}

// ######################################################################################################

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

static void testLexer(Lexer lexer)
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Error: ./webser [configuration file]" DEFAULT << std::endl;
        return (ERROR);
    }

    std::ifstream file(argv[1]);
    openFile(file, argv[1]);

    Lexer lexer(file);

    testLexer(lexer);

    closeFile(file);

    return (SUCCESS);
}
