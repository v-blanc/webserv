/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:10:43 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 14:22:14 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "colors.h"
#include "structConfig.h"

// to_string.cpp
template <typename T>
std::string to_string(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


// parseConfErrorMessage.cpp
std::string getTimeOfDay();

void throwSafeOpenFileError(std::ifstream &file, std::string fileName);
void throwUnexpectedType(Token::Type type, int line, std::string fileName);
void throwUnexpectedEOF(int line, std::string fileName);
void throwInvalidNumberOfArguments(std::string directive, std::string fileName, std::string line);
void throwDirectiveIsDuplicate(std::string directive, std::string fileName, std::string line);
void throwInvalidAutoindexValue(std::string value, std::string fileName, std::string line);
void throwInvalidClientMaxValue(std::string directive, std::string fileName, std::string line);
void throwDuplicateValues(std::string directive, std::string value, std::string fileName, std::string line);
void throwUnknownDirective(std::string directive, std::string fileName, std::string line);
void handleListenFormatError(std::string listen, std::string fileName, std::string line);

#endif
