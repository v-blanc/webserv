/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:52 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/07 16:53:46 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::string &request) : _contentLength(0), _connection(true)
{
    try
    {
        this->parseRequest(request);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << RED << e.what() << DEFAULT << std::endl;
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << RED << e.what() << DEFAULT << '\n';
        return;
    }
}

HTTPRequest::~HTTPRequest()
{
}

static std::vector<std::string> getHTTPLines(std::string &request)
{
    std::vector<std::string> lines;
    std::string currLine;

    for (std::size_t i = 0; i < request.size(); i++)
    {
        if (request.at(i) == '\r')
        {
            if (i + 1 < request.size() && request.at(i + 1) == '\n')
            {
                lines.push_back(currLine);
                currLine.clear();
                i++;
                continue;
            }
        }

        currLine += request.at(i);
    }

    if (!currLine.empty())
        lines.push_back(currLine);

    return lines;
}

void HTTPRequest::parseFirstLine(std::string &firstLine)
{
    std::size_t prevPos = 0, pos = firstLine.find(' ');

    if (pos != std::string::npos)
    {
        std::string method = firstLine.substr(prevPos, pos - prevPos);
        if (method == "GET" || method == "POST" || method == "DELETE")
            this->_method = method;
        else
            throw std::runtime_error("Unknown method");
    }
    else
        throw std::runtime_error("HTTP Request wrong format");

    prevPos = pos + 1;
    pos = firstLine.find(' ', prevPos);

    if (pos != std::string::npos)
    {
        std::string path = firstLine.substr(prevPos, pos - prevPos);
        if (pos != prevPos)
            this->_path = path;
        else
            throw std::runtime_error("HTTP Request path wrong format");
    }
    else
        throw std::runtime_error("HTTP Request wrong format");

    if (firstLine.substr(pos + 1, firstLine.size() - pos) != "HTTP/1.1")
        throw std::runtime_error("HTTP Request HTTP version wrong format");
}

void HTTPRequest::parseHeader(std::string &line)
{
    std::size_t pos = line.find(':');

    if (pos == std::string::npos)
        throw std::runtime_error("HTTP Request wrong format");

    std::string headerName = line.substr(0, pos);

    if (line.at(pos + 1) == ' ')
        pos++;

    if (headerName == "Host")
        this->_host = line.substr(pos + 1);
    else if (headerName == "Content-Length")
    {
        std::stringstream ss(line.substr(pos + 1));
        ss >> this->_contentLength;

        if (ss.fail())
            throw std::runtime_error("HTTP Request error during stringstream");
    }
    else if (headerName == "Connection")
    {
        if (line.substr(pos + 1) == "keep-alive")
            this->_connection = true;
        else if (line.substr(pos + 1) == "close")
            this->_connection = false;
        else
            throw std::runtime_error("HTTP Request wrong format");
    }
}

void HTTPRequest::parseRequest(std::string &request)
{
    std::vector<std::string> lines = getHTTPLines(request);

    // TODO: Debug
    std::cout << "******** Parse HTTP Request: ********" << std::endl;
    for (std::size_t j = 0; j < lines.size(); j++)
        std::cout << "[" + lines[j] << "]" << std::endl;
    std::cout << std::endl;

    // First Line
    if (lines.size() > 1)
        parseFirstLine(lines.at(0));
    else
        throw std::runtime_error("HTTP Request wrong format (empty request)");

    // Header
    std::size_t i = 1;
    while (i < lines.size() && !lines.at(i).empty())
        parseHeader(lines.at(i++));

    // Body
    i++;
    while (i < lines.size())
        this->_body += lines.at(i++) + "\r\n";
}

void printHTTPRequest(HTTPRequest &request)
{
    std::string pad(" ", 4);

    std::cout << "First line:" << std::endl;
    std::cout << pad << "Method: \'" << request.getMethod() << "\'" << std::endl;
    std::cout << pad << "Path: \'" << request.getPath() << "\'" << std::endl;
    std::cout << std::endl;

    std::cout << "Header:" << std::endl;
    std::cout << pad << "Host: \'" << request.getHost() << "\'" << std::endl;
    std::cout << pad << "ContentLength: \'" << request.getContentLength() << "\'" << std::endl;
    std::cout << pad << "Connection: \'" << request.getConnection() << "\'" << std::endl;
    std::cout << std::endl;

    std::cout << "Body:" << std::endl;
    std::cout << "\"" << request.getBody() << "\"" << std::endl;
}
