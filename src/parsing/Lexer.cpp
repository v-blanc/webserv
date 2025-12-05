/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 12:00:57 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 12:23:42 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

Lexer::Lexer(const char *fileName) : _fileName(fileName), _file(fileName), _line(0)
{
    this->openFile();

    this->nextChar();
    while (this->_c != EOF)
    {
        std::cout << _c;
        this->nextChar();
    }
}

Lexer::~Lexer()
{
    this->closeFile();
}

int Lexer::openFile(void)
{
    if (!this->_file.is_open())
    {
        std::cerr << RED "Error with ‘" ITALIC << this->_fileName << DEFAULT RED "’ configuration file: \"" ITALIC;
        if (this->_file.fail())
            std::cerr << strerror(errno);
        std::cerr << "\"" DEFAULT << std::endl;
        return (ERROR);
    }
    std::cout << GREEN "Configuration file ‘" ITALIC << this->_fileName << DEFAULT GREEN "’ was opened sucessfully!" DEFAULT << std::endl;
    return (SUCCESS);
}

void Lexer::closeFile(void)
{
    this->_file.close();
}

void Lexer::nextChar()
{
    this->_c = this->_file.get();
    if (this->_c == '\n')
        this->_line++;
    if (this->_c == std::char_traits<char>::eof())
        this->_c = EOF;
}
