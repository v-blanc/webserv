/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 18:20:07 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Error: ./webser [configuration file]" DEFAULT << std::endl;
        return (ERROR);
    }

    std::ifstream file(argv[1]);
    openFile(file, argv[1]);

    // Lexer lexer(file);
    
    try
    {
        Parser p(file);
        Node root = p.parse();
        printNode(root);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    // printLexer(lexer);

    closeFile(file);

    return (SUCCESS);
}
