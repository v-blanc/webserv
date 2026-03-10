/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest_parseRequest.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 16:19:40 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/10 09:01:39 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

std::vector<std::string> getHTTPLines(std::string &request)
{
	std::vector<std::string> lines;
	std::string currLine;
	bool headersDone = false;

	for (std::size_t i = 0; i < request.size(); i++)
	{
		if (request.at(i) == '\r')
		{
			if (i + 1 < request.size() && request.at(i + 1) == '\n')
			{
				if (!headersDone)
				{
					if (!currLine.empty())
						lines.push_back(currLine);
					currLine.clear();
					if (i + 3 < request.size() && request.at(i + 2) == '\r' && request.at(i + 3) == '\n')
					{
						lines.push_back("\r\n\r\n");
						headersDone = true;
						i += 3;
						continue;
					}
				}
				else
				{
					currLine += '\r';
					currLine += '\n';
					++i;
					continue;
				}
				++i;
				continue;
			}
		}
		currLine += request.at(i);
	}
	if (!currLine.empty())
		lines.push_back(currLine);
	return (lines);
}

void HTTPRequest::parseFirstLine(std::string &firstLine)
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
        std::string path = firstLine.substr(prevPos, pos - prevPos);
        
        //path = resolvePath(2, this->_serverConfig.getRoot().c_str(), path.c_str());

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

    else if (headerName == "Host")
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

static std::string extractFilename(const std::string &line)
{
    std::size_t pos = line.find("filename=\"");
    if (pos == std::string::npos)
        return ("");
    pos += 10; // skip: filename="
    std::size_t end = line.find('"', pos);
    if (end == std::string::npos)
        return ("");
    return (line.substr(pos, end - pos));
}

void HTTPRequest::parseRequest(std::string &request)
{
    bool isLastLine;
    std::vector<std::string> lines = getHTTPLines(request);

    if (lines.size() > 1)
        parseFirstLine(lines.at(0));
    else
        throw StatusException("400", "HTTP Request wrong format");

    std::size_t i = 1;
    while (i < lines.size() && lines.at(i) != "\r\n\r\n")
    {
        if (lines.at(i).size() >= 2 && lines.at(i)[0] == '-' && lines.at(i)[1] == '-')
        {
            i++;
            continue;
        }

        isLastLine = (i + 1 == lines.size());
        if (!parseHeader(lines.at(i), this->_host != "", isLastLine))
            throw StatusException("400", "Bad Request");
        i++;
    }

    if (this->_host.empty())
        throw StatusException("400", "Bad Request");

    i++; // skip the \r\n\r\n separator

    while (i < lines.size())
    {
        if (lines.at(i) == "\r\n\r\n")
        {
            i++;
            continue;
        }

        if (!this->_body.empty())
            this->_body += "\r\n";
        this->_body += lines.at(i);
        i++;
    }

    if (this->_fileName.empty() &&
        this->_contentType.find("multipart/form-data") != std::string::npos)
    {
        std::istringstream bodyStream(this->_body);
        std::string bodyLine;

        while (std::getline(bodyStream, bodyLine))
        {
            if (!bodyLine.empty() && bodyLine[bodyLine.size() - 1] == '\r')
                bodyLine.erase(bodyLine.size() - 1);

            if (bodyLine.find("Content-Disposition:") != std::string::npos)
            {
                std::string fn = extractFilename(bodyLine);

                if (!fn.empty())
                {
                    this->_fileName = fn;
                    break;
                }
            }
        }
    }
}
