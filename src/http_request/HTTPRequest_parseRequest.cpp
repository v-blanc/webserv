/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest_parseRequest.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 16:19:40 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/30 18:41:42 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http_request/HTTPRequest.hpp"

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
                if (!currLine.empty())
                    lines.push_back(currLine);
                currLine.clear();
                i++;
                continue;
            }
        }
        currLine += request.at(i);
    }
    //  std::cout<<"my thing: "<<lines.at(0)<<std::endl;
    if (!currLine.empty())
        lines.push_back(currLine);
    return lines;
}

void HTTPRequest:: parseFirstLine(std::string &firstLine)
{
    std::size_t prevPos = 0, pos = firstLine.find(' ');

    if (pos != 0 && pos != std::string::npos)
    {
        std::string method = firstLine.substr(prevPos, pos);
        if (method == "GET" || method == "POST" || method == "DELETE")
            this->_method = method;
        else
            throw StatusException("405", "Method Not Allowed");
    }
    else
        throw StatusException("400", "Bad Request");
    prevPos = pos + 1;
    pos = firstLine.find(' ', prevPos);
    if (pos != std::string::npos)
    {
        std::string const path = firstLine.substr(prevPos, pos - prevPos);
        if (pos != prevPos)
        {
            this->_path = path;
            std::size_t const q = path.find('?');
            if (q != std::string::npos)
                this->_queryString = path.substr(q + 1);
            else
                this->_queryString.clear();
        }
        else
            throw StatusException("400", "HTTP Request path wrong format");
    }
    else
        throw StatusException("400", "HTTP Request wrong format");

    if (firstLine.substr(pos + 1, firstLine.size() - pos) != "HTTP/1.1")
        throw StatusException("505", "HTTP Version Not Supported");
}

bool HTTPRequest::parseHeader(std::string &line, bool checkedHost, bool isLastLine)
{
    std::size_t pos = line.find(':');
    if (pos == std::string::npos)
        throw StatusException("400", "HTTP Request wrong format");
    std::string headerName = line.substr(0, pos);
    if (line.at(pos + 1) == ' ')
        pos++;
    if (headerName != "Host" && !checkedHost && isLastLine)
        return (false);
    if (headerName == "Host")
        this->_host = line.substr(pos + 1);
    else if (headerName == "Content-Length")
    {
        std::stringstream ss(line.substr(pos + 1));
        ss >> this->_contentLength;
        if (ss.fail())
            throw StatusException("400", "HTTP Request error during stringstream");
    }
    else if (headerName == "Content-Type")
        this->_contentType = line.substr(pos + 1);
    else if (headerName == "Connection")
    {
        if (line.substr(pos + 1) == "keep-alive")
            this->_connection = true;
        else if (line.substr(pos + 1) == "close")
            this->_connection = false;
        else
            throw std::runtime_error("HTTP Request wrong format");
    }
    else if (headerName == "Transfer-Encoding")
    {
        if (line.substr(pos + 1) == "chunked")
            this->_isChunked = true;
    }
    return (true);
}

void HTTPRequest::parseRequest(std::string &request)
{
    bool isLastLine;
    std::vector<std::string> lines = getHTTPLines(request);
    //  std::cout<<"my thing: "<<lines.at(0)<<std::endl;
    // TODO: Debug
    // std::cout << "******** Parse HTTP Request: ********" << std::endl;
    // for (std::size_t j = 0; j < lines.size(); j++)
    //     std::cout << "[" + lines[j] << "]" << std::endl;
    // std::cout << std::endl;

    // First Line
    if (lines.size() > 1)
    {
        try
        {
            parseFirstLine(lines.at(0));

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
    else
        throw StatusException("400", "HTTP Request wrong format");
    // Header
    std::size_t i = 1;
    // std::cout<<"bool test: "<<test<<std::endl;
    while (i < lines.size() && !lines.at(i).empty())
    {
        isLastLine = (i + 1  == lines.size());
        if (!parseHeader(lines.at(i), this->_host != "", isLastLine))
            throw StatusException("400", "Bad Request");
        i++;
    }
    // Body
    i++;
    while (i < lines.size())
        this->_body += lines.at(i++) + "\r\n";
}
