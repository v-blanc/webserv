/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:10:43 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/17 18:50:40 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "define.h"
#include "colors.h"
#include "structConfig.h"

// toString.cpp
template <typename T>
std::string toString(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// getTimeOfDay.cpp
std::string getTimeOfDay();

// getLocalFileContent.cpp
std::string getLocalFileContent(const std::string &fileName);

// isInvalidPath.cpp
bool isInvalidPath(const std::string &path);

// generateAutoindexHTML.cpp
std::string generateAutoindexHTML(const std::string &path);

// errorMessageConfig.cpp
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
void throwErrorPageInvalidValue(std::string value, std::string fileName, std::string line);

// errorMessageServer.cpp
void handleBindError(std::string listen, std::string fileName);
void throwMajorIssueCreatingServer(std::string serverName);

#endif
