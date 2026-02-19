/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest_parseRequest.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 16:19:40 by vblanc            #+#    #+#             */
/*   Updated: 2026/02/19 17:32:25 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/http_request/HTTPRequest.hpp"

static std::vector<std::string> getHTTPLines(std::string &request)
{
    std::vector<std::string> lines;
    std::string currLine;
    int realBody = 0;

    for (std::size_t i = 0; i < request.size(); i++)
    {
        if (request.at(i) == '\r')
        {
            if (i + 1 < request.size() && request.at(i + 1) == '\n')
            {
                if (!currLine.empty())
                    lines.push_back(currLine);
                currLine.clear();
                if (i + 3 < request.size() && request.at(i + 2) == '\r' && request.at(i + 3) == '\n')
                {
                    if (realBody == 1)
                    {
                        std::vector<std::string>::iterator it = std::find(lines.begin(), lines.end(), "\r\n\r\n");
                        if (it != lines.end())
                            lines.erase(it);
                    }
                    else
                        realBody++;
                    lines.push_back("\r\n\r\n");
                }
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
    std::cout<<"vasy ma ligne: "<<line<<std::endl;
    std::size_t pos = line.find(':');
    if (pos == std::string::npos)
    {
        std::cout<<"my line: "<<line<<std::endl;
        throw StatusException("400", "HTTP Request wrong format");
    }
    std::string headerName = line.substr(0, pos);
    if (line.at(pos + 1) == ' ')
        pos++;
    if (headerName != "Host" && !checkedHost && isLastLine)
        return (false);
    if (headerName == "Host")
        this->_host = line.substr(pos + 1);
    else if (headerName == "Content-Disposition")
    {
        std::size_t namePos =  line.find_last_of('=');
        // std::cout<<"filenamoo: "<<line.substr(namePos + 2, line.size() - (namePos + 3))<<std::endl;
        this->_fileName = line.substr(namePos + 2, line.size() - (namePos + 3));
    }
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
	else if (headerName == "Cookie")
	{
		std::string cookieStr = line.substr(pos + 1);
    
		while (!cookieStr.empty())
		{
			std::size_t semicolon_pos = cookieStr.find(';');
			std::string pair;
			if (semicolon_pos != std::string::npos)
			{
				pair = cookieStr.substr(0, semicolon_pos);
				cookieStr = cookieStr.substr(semicolon_pos + 1);
			}
			else
			{
				pair = cookieStr;
				cookieStr.clear();
			}
			std::size_t start = pair.find_first_not_of(' ');
			if (start == std::string::npos)
				continue;
			pair = pair.substr(start);
			std::size_t equals_sign_pos = pair.find('=');
			if (equals_sign_pos != std::string::npos)
				this->_cookies[pair.substr(0, equals_sign_pos)] = pair.substr(equals_sign_pos + 1);
		}
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
    // std::cout<<"line size: "<<lines.size()<<std::endl;
    // if (lines.size() > 18)
    //     std::cout<<"line 18: "<<lines.at(18)<<std::endl;
    while (i < lines.size() && !lines.at(i).empty())
    {
        if (lines.at(i).at(0) == '-' && lines.at(i).at(1) == '-')
        {
            i++;
            continue;
        }
        isLastLine = (i + 1  == lines.size());
        if (lines.at(i) == "\r\n\r\n")
            break;
        if (!parseHeader(lines.at(i), this->_host != "", isLastLine))
            throw StatusException("400", "Bad Request");
        i++;
    }
    // Body
    i++;
    while (i < lines.size())
        this->_body += lines.at(i++) + "\r\n";
}
