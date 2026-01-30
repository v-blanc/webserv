/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:52 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/30 18:51:14 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http_request/HTTPRequest.hpp"

HTTPRequest::HTTPRequest(ServerConfig &serverConfig, std::string &request, std::string &responseBuff) : _isValidRequest(false), _contentLength(0), _connection(true)
{
    try
    {
        this->parseRequest(request);
        this->_isValidRequest = true;

        HTTPResponse myResponse(*this, "", serverConfig, "");
        // TODO: to test
        responseBuff = myResponse.getResponse();
    }
    catch (const StatusException &e)
    {
        HTTPResponse badResponse(*this, e.getStatus(), e.getMessage());
        responseBuff = badResponse.getResponse();
    }
}

HTTPRequest::~HTTPRequest()
{
}

std::string HTTPRequest::getPathWithoutQuery() const
{
    std::size_t q = this->_path.find('?');
    if (q == std::string::npos)
        return (this->_path);
    return (this->_path.substr(0, q));
}

bool HTTPRequest::isCgiExtension() const

{
    std::string lowerPath(this->getPathWithoutQuery());

    for (size_t i = 0; i < lowerPath.size(); ++i)
        lowerPath[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowerPath[i])));
    if (lowerPath.size() >= 4 && lowerPath.rfind(".php") == lowerPath.size() - 4)
        return (true);
    if (lowerPath.size() >= 3 && lowerPath.rfind(".py") == lowerPath.size() - 3)
        return (true);
    if (lowerPath.size() >= 4 && lowerPath.rfind(".cgi") == lowerPath.size() - 4)
        return (true);
    return (false);
}

void HTTPRequest::sendCgiStubResponse(int &clientFd) const

{
    std::string resp;
    std::string const body = "CGI detected for: " + this->_path + "\n";

    resp = "HTTP/1.1 501 Not Implemented\r\n";
    resp += "Content-Type: text/plain\r\n";
    resp += "Content-Length: ";
    resp += toString(body.size());
    resp += "\r\n\r\n";
    resp += body;
    send(clientFd, resp.c_str(), resp.size(), MSG_NOSIGNAL);
}

std::string HTTPRequest::getNormalizedExtensionFromPath() const
{
    std::string lowerPath(this->getPathWithoutQuery());

    for (size_t i = 0; i < lowerPath.size(); ++i)
        lowerPath[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowerPath[i])));

    std::size_t slash = lowerPath.find_last_of('/');
    std::string base = (slash == std::string::npos) ? lowerPath : lowerPath.substr(slash + 1);
    std::size_t dot = base.find_last_of('.');
    if (dot == std::string::npos || dot == 0 || dot == base.size() - 1)
        return ("");
    return (base.substr(dot));
}

static std::string normalizeExt(std::string ext)

{
    for (size_t i = 0; i < ext.size(); ++i)
        ext[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(ext[i])));
    if (!ext.empty() && ext[0] != '.')
        ext = "." + ext;
    return (ext);
}

bool HTTPRequest::resolveCgiInterpreter(const std::vector<stringPair> &cgiHandlers, std::string &interpreter) const

{
    std::string const ext = this->getNormalizedExtensionFromPath();

    if (ext.empty())
        return (false);
    if (ext != ".php" && ext != ".py" && ext != ".cgi")
        return (false);
    for (size_t i = 0; i < cgiHandlers.size(); ++i)
    {
        if (normalizeExt(cgiHandlers[i].first) == ext)
        {
            interpreter = cgiHandlers[i].second;
            return (true);
        }
    }
    return (false);
}

void HTTPRequest::debugResponseWithCgiHandlers(int &clientFd, const std::vector<stringPair> &cgiHandlers)
{
    std::string interpreter;

    if (this->resolveCgiInterpreter(cgiHandlers, interpreter))
    {
        std::string body = "CGI selected\n";
        body += "path: " + this->_path + "\n";
        body += "interpreter: " + interpreter + "\n";

        std::string resp = "HTTP/1.1 501 Not Implemented\r\n";
        resp += "Content-Type: text/plain\r\n";
        resp += "Content-Length: " + toString(body.size()) + "\r\n\r\n";
        resp += body;
        send(clientFd, resp.c_str(), resp.size(), MSG_NOSIGNAL);
        return;
    }
    this->debugStandardReponse(clientFd);
}

void HTTPRequest::debugStandardReponse(int &clientFd)
{
    if (!this->_isValidRequest)
        return;

    std::string sendBuf = "HTTP/1.1 200 OK\r\nLocation: http://localhost:8080/\r\nContent-Length: ";
    std::string fileName = "www" + this->_path;

    if (this->isCgiExtension())
    {
        this->sendCgiStubResponse(clientFd);
        return;
    }
    if (this->_path == "/")
        fileName.append("index.html");

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