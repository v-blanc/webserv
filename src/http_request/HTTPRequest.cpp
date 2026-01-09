/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:52 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/17 18:56:33 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(std::string &request) : _isValidRequest(false), _contentLength(0), _connection(true)
{
    try
    {
        this->parseRequest(request);
        this->_isValidRequest = true;
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

void HTTPRequest::debugStandardReponse(int &clientFd)
{
    if (!this->_isValidRequest)
        return;

    // Avoid Chrome duplicates
    if (this->_path.find("favicon.ico") != std::string::npos)
        return;

    std::string sendBuf = "HTTP/1.1 200 OK\r\nLocation: http://localhost:8080/\r\nContent-Length: ";
    std::string fileName = "www" + this->_path;

    if (this->_path == "/")
        fileName.append("index.html");

    // std::cout << "request: \n\"" << request << "\"" << std::endl;

    if (isInvalidPath(fileName))
    {
        std::cerr << "Invalid path: contain invalid " << std::endl;
        return;
    }

    std::string content = getLocalFileContent(fileName);

    sendBuf.append(toString(content.size()));
    sendBuf.append("\r\n\r\n");
    sendBuf.append(content);

    send(clientFd, sendBuf.c_str(), sendBuf.size(), MSG_NOSIGNAL);

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

        if (!ss.eof() || ss.fail())
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
    // std::cout << "******** Parse HTTP Request: ********" << std::endl;
    // for (std::size_t j = 0; j < lines.size(); j++)
    //     std::cout << "[" + lines[j] << "]" << std::endl;
    // std::cout << std::endl;

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
    std::string pad(4, ' ');

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
