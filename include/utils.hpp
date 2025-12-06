/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:10:43 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 15:43:35 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "colors.h"
#include "configStructs.h"
#include <ctime>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// to_string.cpp
template <typename T>
std::string to_string(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// parseConfErrorMessage.cpp
void throwSafeOpenFileError(std::ifstream &file, std::string fileName);
void throwUnexpectedType(Token::Type type, int line, std::string fileName);
void throwUnexpectedEOF(int line, std::string fileName);
void throwInvalidNumberOfArguments(std::string directive, std::string fileName, std::string line);
void throwDirectiveIsDuplicate(std::string directive, std::string fileName, std::string line);
void throwInvalidAutoindexValue(std::string value, std::string fileName, std::string line);
void throwInvalidClientMaxValue(std::string directive, std::string fileName, std::string line);
void throwUnknownDirective(std::string directive, std::string fileName, std::string line);

#endif
