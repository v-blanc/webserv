/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 14:57:52 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/10 01:08:32 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http_request/HTTPRequest.hpp"

HTTPRequest::HTTPRequest(ServerConfig &serverConfig, std::string &request, std::string &responseBuff, SessionManager &sessionManager) : _isValidRequest(false), _contentLength(0), _connection(true), _isChunked(false)
{
    try
    {
        this->parseRequest(request);
        this->_isValidRequest = true;
        // this->debugPrintSession(sessionManager); // DEBUG
        HTTPResponse myResponse(*this, "", serverConfig, "", sessionManager);
        responseBuff = myResponse.getResponse();
    }
    catch (const StatusException &e)
    {
        HTTPResponse badResponse(*this, e.getStatus(), serverConfig, e.getMessage(), sessionManager);
        responseBuff = badResponse.getResponse();
    }
}

void HTTPRequest::debugPrintSession(const SessionManager &sessionManager) const
{
    for (std::map<std::string, std::string>::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
        std::cout << "[Cookie] " << it->first << " = " << it->second << std::endl;

    std::string sessionId = this->getCookie("session_id");
    if (sessionId.empty())
        return;
    if (sessionManager.sessionExists(sessionId))
    {
        SessionData data = sessionManager.getSessionData(sessionId);
        char createdAtStr[20];
        char lastAccessStr[20];

        std::strftime(createdAtStr, sizeof(createdAtStr), "%Y-%m-%d %H:%M:%S", std::localtime(&data.createdAt));
        std::strftime(lastAccessStr, sizeof(lastAccessStr), "%Y-%m-%d %H:%M:%S", std::localtime(&data.lastAccess));

        std::cout << "[Session] id=" << sessionId << std::endl;
        std::cout << "[Session] createdAt=" << createdAtStr << " lastAccess=" << lastAccessStr << std::endl;

        if (data.values.empty())
            std::cout << "[Session] values: (empty)" << std::endl;
        else
        {
            for (std::map<std::string, std::string>::const_iterator vit = data.values.begin(); vit != data.values.end(); ++vit)
                std::cout << "[Session] values: " << vit->first << " = " << vit->second << std::endl;
        }
    }
    else
        std::cout << "[Session] unknown session_id (will create a new one)" << std::endl;
}

HTTPRequest::~HTTPRequest()
{
}

std::string HTTPRequest::getPathWithoutQuery() const
{
    std::size_t q = this->_path.find('?');
    std::string path;
    if (q == std::string::npos)
        path = this->_path;
    else
        path = this->_path.substr(0, q);
    std::string normalized;
    for (std::size_t i = 0; i < path.size(); ++i)
    {
        if (path[i] == '/' && !normalized.empty() && normalized[normalized.size() - 1] == '/')
            continue;
        normalized += path[i];
    }
    return (normalized);
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
    if (lowerPath.size() >= 3 && lowerPath.rfind(".pl") == lowerPath.size() - 3)
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
    if (ext != ".php" && ext != ".py" && ext != ".pl")
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

std::string HTTPRequest::getCookie(const std::string &name) const
{
    std::map<std::string, std::string>::const_iterator it = _cookies.find(name);
    if (it != _cookies.end())
        return (it->second);
    return ("");
}

void HTTPRequest::pushBackCookies(const std::string key, const std::string value)
{
    _cookies[key] = value;
}
