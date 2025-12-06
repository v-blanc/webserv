/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseConfErrorMessage.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:15:31 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 18:01:06 by vblanc           ###   ########.fr       */
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

static std::string getTimeOfDay()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S [emerg] : ", t);

    return (buffer);
}

void throwSafeOpenFileError(std::ifstream &file, std::string fileName)
{
    std::string errorMessage = RED "Error with ‘" ITALIC + fileName + DEFAULT RED "’ configuration file: \"" ITALIC;

    if (file.fail())
        errorMessage += strerror(errno);
    errorMessage += "\"" DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwUnexpectedType(Token::Type type, int line, std::string fileName)
{
    std::stringstream ss;
    ss << line;

    std::string errorMessage = RED + getTimeOfDay() + "unexpected \"" + tokenType(type) + "\" in " + fileName + ":" + ss.str() + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwUnexpectedEOF(int line, std::string fileName)
{
    std::stringstream ss;
    ss << line;

    std::string errorMessage = RED + getTimeOfDay() + "unexpected end of file, expecting \"}\" in " + fileName + ":" + ss.str() + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwInvalidNumberOfArguments(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + "invalid number of arguments in \"" + directive + "\" in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwDirectiveIsDuplicate(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + "\"" + directive + "\" directive is duplicate in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwInvalidAutoindexValue(std::string value, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + "invalid value \"" + value + "\" in \"autoindex\" directive, it must be \"on\" or \"off\" in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwInvalidClientMaxValue(std::string directive, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + "\"" + directive + "\" directive invalid value in ";
    errorMessage += fileName + ":" + line + DEFAULT;

    throw std::runtime_error(errorMessage);
}

void throwDuplicateValues(std::string directive, std::string value, std::string fileName, std::string line)
{
    std::string errorMessage = RED + getTimeOfDay() + "duplicate " + directive + " \"" + value + "\" in ";
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
        errorMessage = RED + getTimeOfDay() + "\"" + directive + "\" directive is not allowed here in ";
        errorMessage += fileName + ":" + line + DEFAULT;
    }
    else
    {
        errorMessage = RED + getTimeOfDay() + "unknown directive \"" + directive + "\" in ";
        errorMessage += fileName + ":" + line + DEFAULT;
    }
    throw std::runtime_error(errorMessage);
}
