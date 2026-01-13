/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yafahfou <yafahfou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:52 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/13 18:01:20 by yafahfou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(ServerConfig &serverConfig, std::string &request) : _serverConfig(serverConfig), _isValidRequest(false), _contentLength(0), _connection(true)
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

// void HTTPRequest::debugStandardReponse(int &clientFd)
{
    if (!this->_isValidRequest)
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
}

// void printHTTPRequest(HTTPRequest &request)
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
