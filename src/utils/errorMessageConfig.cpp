/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorMessageConfig.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:15:31 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 17:53:23 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

static const std::string tokenType(Token::Type &type)
{
    switch (type)
    {
    case Token::LBRACE:
        return "{";
    case Token::RBRACE:
        return "}";
    case Token::SEMICOLON:
        return ";";
    default:
        return "unknown";
    }
}

void throwSafeOpenFileError(std::ifstream &file, std::string fileName)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : Error with ‘" ITALIC + fileName + DEFAULT RED "’ configuration file: \"" ITALIC;

    if (file.fail())
        errorMessage += strerror(errno);
    errorMessage += "\"" DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwUnexpectedType(Token::Type type, int line, std::string fileName)
{
    std::stringstream ss;
    ss << line;

    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : unexpected \"" + tokenType(type) + "\" in " + fileName + ":" + ss.str() + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwUnexpectedEOF(int line, std::string fileName)
{
    std::stringstream ss;
    ss << line;

    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : unexpected end of file, expecting \"}\" in " + fileName + ":" + ss.str() + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwInvalidNumberOfArguments(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : invalid number of arguments in \"" + directive + "\" in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwDirectiveIsDuplicate(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : \"" + directive + "\" directive is duplicate in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwInvalidAutoindexValue(std::string value, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : invalid value \"" + value + "\" in \"autoindex\" directive, it must be \"on\" or \"off\" in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwInvalidClientMaxValue(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : \"" + directive + "\" directive invalid value in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwDuplicateValues(std::string directive, std::string value, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + " [emerg] : duplicate " + directive + " \"" + value + "\" in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

static bool isKnownDirective(std::string &directive)
{
    std::string known[10] = {"server", "autoindex", "client_max_body_size", "root", "index",
                             "error_page", "location", "listen", "server_name", "cgi_handler"};

    for (std::size_t i = 0; i < 10; i++)
        if (directive == known[i])
            return (true);
    return (false);
}

void throwUnknownDirective(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage;
    if (isKnownDirective(directive))
    {
        errorMessage = RED + getTimeOfDay() + " [emerg] : \"" + directive + "\" directive is not allowed here in ";
        errorMessage += fileName + ":" + line + DEFAULT;
    }
    else
    {
        errorMessage = RED + getTimeOfDay() + " [emerg] : unknown directive \"" + directive + "\" in ";
        errorMessage += fileName + ":" + line + DEFAULT;
    }
    throw std::runtime_error(errorMessage);
}

void handleListenFormatError(std::string listen, std::string fileName, std::string line)
{
    std::string errorMessage = YELLOW + getTimeOfDay() + " [warning] : host not found in \"" + listen;
    errorMessage += +"\" of the \"listen\" directive in " + fileName + ":" + line + DEFAULT;

    std::cerr << errorMessage << std::endl;
}
