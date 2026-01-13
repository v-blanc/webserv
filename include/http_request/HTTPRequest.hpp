/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yafahfou <yafahfou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:04 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/13 18:01:28 by yafahfou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "ServerConfig.hpp"
#include "utils.hpp"

class HTTPRequest
{
public:
    HTTPRequest(ServerConfig &serverConfig, std::string &request);
    ~HTTPRequest();

    // Getter
    bool getIsValidRequest() const { return this->_isValidRequest; };
    std::string getMethod() const { return this->_method; };
    std::string getPath() const { return this->_path; };
    std::string getHost() const { return this->_host; };
    long long getContentLength() const { return this->_contentLength; };
    bool getConnection() const { return this->_connection; }; // ?
    std::string getBody() const { return this->_body; };

    // Execute
    // virtual void generateResponse() = 0;

    // TODO: Debug
    // void debugStandardReponse(int &clientFd);

private:
    ServerConfig _serverConfig;

    bool _isValidRequest;

    // Start Line
    std::string _method;
    std::string _path;

    // Header
    std::string _host;
    long long _contentLength;
    bool _connection; // ? (keep-alive or close)

    // Body
    std::string _body;

    // Parsing
    void parseFirstLine(std::string &firstLine);
    void parseHeader(std::string &line);
    void parseRequest(std::string &request);
};

// TODO: Debug
// void printHTTPRequest(HTTPRequest &request);

#endif
