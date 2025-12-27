/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:04 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/19 16:28:17 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "utils.hpp"

class HTTPRequest
{
public:
    HTTPRequest(std::string &request);
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
    void debugStandardReponse(int &clientFd);
    void debugResponseWithCgiHandlers(int &clientFd, const std::vector<stringPair>& cgiHandlers);

private:
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

    bool        isCgiExtension(void) const;
    void        sendCgiStubResponse(int &clientFd) const;
    std::string getNormalizedExtensionFromPath(void) const;
    bool        resolveCgiInterpreter(const std::vector<stringPair>& cgiHandlers, std::string& interpreter) const;
};

// TODO: Debug
void printHTTPRequest(HTTPRequest &request);

#endif
