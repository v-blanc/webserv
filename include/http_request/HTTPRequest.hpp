/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:04 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/21 16:30:06 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "ServerConfig.hpp"
#include "utils.hpp"
#include "HTTPResponse.hpp"
class HTTPRequest
{
public:
    HTTPRequest(ServerConfig &serverConfig, std::string &request, std::string &responseBuff);
    ~HTTPRequest();

    // Getter
    bool getIsValidRequest() const { return this->_isValidRequest; };
    std::string getMethod() const { return this->_method; };
    const std::string &getPath() const { return this->_path; };
    const std::string &getQueryString() const { return this->_queryString; };
    std::string getPathWithoutQuery() const;
    std::string getHost() const { return this->_host; };
    long long getContentLength() const { return this->_contentLength; };
    std::string getContentType() const { return this->_contentType; };
    bool getConnection() const { return this->_connection; }; // ?
    std::string getBody() const { return this->_body; };
    bool isChunked() const { return this->_isChunked; };


    class StatusException: public std::exception {
        private:
            std::string _status;
            std::string _message;
        public:
            StatusException(std::string status, std::string message):_status(status), _message(message) {}
            virtual ~StatusException() throw() {}
            virtual const char *what() const throw() {return _message.c_str();}
            const std::string &getStatus() const  {return _status;}
    };
    // Execute
    // virtual void generateResponse() = 0;

    // TODO: Debug
    void debugStandardReponse(int &clientFd);
    void debugResponseWithCgiHandlers(int &clientFd, const std::vector<stringPair> &cgiHandlers);
    bool resolveCgiInterpreter(const std::vector<stringPair> &cgiHandlers, std::string &interpreter) const;

private:

    bool _isValidRequest;
    std::pair<std::string, std::string> codeStatus;

    // Start Line
    std::string _method;
    std::string _path;
    std::string _queryString;

    // Header
    std::string _host;
    long long _contentLength;
    std::string _contentType;
    bool _connection; // ? (keep-alive or close)

    // Body
    std::string _body;
    bool _isChunked;

    // Parsing
    void parseFirstLine(std::string &firstLine);
    void parseHeader(std::string &line);
    void parseRequest(std::string &request);

    bool isCgiExtension(void) const;
    void sendCgiStubResponse(int &clientFd) const;
    std::string getNormalizedExtensionFromPath(void) const;
    // (moved to public)
};

// TODO: Debug
// void printHTTPRequest(HTTPRequest &request);

#endif
