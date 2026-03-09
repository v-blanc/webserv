/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveHandlerConfig.tpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 14:21:38 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/09 18:03:57 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <set>

template <typename Config>
void handleAutoindex(Config &config, Node &node, std::string &directive)
{
    if (config.getAutoindex() != -1)
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    if (node.args.at(0) == "on")
        config.setAutoindex(true);
    else if (node.args.at(0) == "off")
        config.setAutoindex(false);
    else
        throwInvalidAutoindexValue(node.args.at(0), config.getFileName(), node.line);
}

template <typename Config>
void handleClientMaxBodySize(Config &config, Node &node, std::string &directive)
{
    if (config.getClientMaxBodySize() != -1)
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    if (!node.args.at(0).empty() && node.args.at(0)[0] == '-')
        throwInvalidClientMaxValue(directive, config.getFileName(), node.line);

    std::stringstream ss(node.args.at(0));
    unsigned long long result;
    ss >> result;
    if (!ss.eof() || ss.fail())
        throwInvalidClientMaxValue(directive, config.getFileName(), node.line);
    config.setClientMaxBodySize(result);
}

template <typename Config>
void handleRootDirective(Config &config, Node &node, std::string &directive)
{
    if (!config.getRoot().empty())
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    config.setRoot(node.args.at(0));
}

template <typename Config>
void handleIndexDirective(Config &config, Node &node)
{
    for (std::size_t i = 0; i < node.args.size(); i++)
        config.pushBackIndex(node.args.at(i));
}

template <typename Config>
void handleErrorPageDirective(Config &config, Node &node, std::string &directive)
{
    if (node.args.size() < 2)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    std::string file = node.args.back();

    for (std::size_t i = 0; i < (node.args.size() - 1); i++)
    {
        if (!node.args.at(i).empty() && node.args.at(i)[0] == '-')
            throwInvalidClientMaxValue(directive, config.getFileName(), node.line);

        std::stringstream ss(node.args.at(i));
        int errorCode;
        ss >> errorCode;

        if (!ss.eof() || ss.fail())
            throwErrorPageInvalidValue(node.args.at(i), config.getFileName(), node.line);
        config.pushBackErrorPage(errorCode, file);
    }
}

template <typename Config>
void handleListenDirective(Config &config, Node &node, std::string &directive)
{
    if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    std::string hostNameStr, portStr;
    uint32_t hostName = 0;
    uint16_t port = 0;

    std::size_t pos = node.args.at(0).find(':');

    if (pos == 0)
    {
        handleListenFormatError(node.args.at(0), config.getFileName(), node.line);
        return;
    }
    else if (pos == std::string::npos) // Only port is given
    {
        hostNameStr.clear();
        portStr = node.args.at(0);

        std::string::const_iterator it = portStr.begin();
        while (it != portStr.end() && std::isdigit(*it))
            it++;
        if (it != portStr.end())
        {
            handleListenFormatError(node.args.at(0), config.getFileName(), node.line);
            return;
        }
    }
    else // IPv4:port is given
    {
        hostNameStr = node.args.at(0).substr(0, pos);
        portStr = node.args.at(0).substr(pos + 1);
    }

    // Handle port
    if (portStr.empty())
    {
        handleListenFormatError(node.args.at(0), config.getFileName(), node.line);
        return;
    }

    std::stringstream ss(portStr);
    ss >> port;
    if (!ss.eof() || ss.fail())
    {
        handleListenFormatError(node.args.at(0), config.getFileName(), node.line);
        return;
    }

    // Handle host name
    if (hostNameStr.empty() || hostNameStr == "*")
        hostName = INADDR_ANY;
    else
    {
        struct addrinfo hints;
        struct addrinfo *res = NULL;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(hostNameStr.c_str(), portStr.c_str(), &hints, &res) != 0)
        {
            handleListenFormatError(node.args.at(0), config.getFileName(), node.line);
            freeaddrinfo(res);
            return;
        }

        // Take the first IPv4 result
        struct addrinfo *rp = NULL;
        for (rp = res; rp != NULL; rp = rp->ai_next)
        {
            if (rp->ai_family != AF_INET)
                continue;
            struct sockaddr_in *sin = (struct sockaddr_in *)rp->ai_addr;
            hostName = sin->sin_addr.s_addr;
            break;
        }

        freeaddrinfo(res);
    }

    config.pushBackListenStr(node.args.at(0));
    config.pushBackListen(std::make_pair(hostName, htons(port)));

    // Check for duplicates
    std::vector<listenPair> listen = config.getListen();
    std::set<listenPair> seenAddr;
    std::set<uint16_t> seenPort;

    for (size_t i = 0; i < listen.size(); i++)
    {
        if (seenAddr.count(listen.at(i)) || seenPort.count(listen.at(i).second))
            throwDuplicateValues(directive, node.args.at(0), config.getFileName(), node.line);
        seenAddr.insert(listen.at(i));
        seenPort.insert(listen.at(i).second);
    }
}

template <typename Config>
void handleServerNameDirective(Config &config, Node &node, std::string &directive)
{
    for (std::size_t i = 0; i < node.args.size(); i++)
        config.pushBackServerName(node.args.at(i));

    std::vector<std::string> serverName = config.getServerName();
    std::set<std::string> seen;

    for (size_t i = 0; i < serverName.size(); ++i)
    {
        if (seen.count(serverName.at(i)))
            throwDuplicateValues(directive, node.args.at(0), config.getFileName(), node.line);
        seen.insert(serverName.at(i));
    }
}

template <typename Config>
void handleCgiHandlerDirective(Config &config, Node &node, std::string &directive)
{
    if (node.args.size() != 2)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    config.pushBackCgiHandler(std::make_pair(node.args.at(0), node.args.at(1)));

    std::vector<stringPair> cgiHandler = config.getCgiHandler();
    std::set<stringPair> seen;

    for (size_t i = 0; i < cgiHandler.size(); ++i)
    {
        if (seen.count(cgiHandler.at(i)))
            throwDuplicateValues(directive, node.args.at(0), config.getFileName(), node.line);
        seen.insert(cgiHandler.at(i));
    }
}

template <typename Config>
void handleUpdloadStoreDirective(Config &config, Node &node, std::string &directive)
{
    if (!config.getUploadStore().empty())
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    config.setUploadStore(node.args.at(0));
}

template <typename Config>
void handleReturnDirective(Config &config, Node &node, std::string &directive)
{
    if (!config.getReturn().empty())
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);

    config.setReturn(node.args.at(0));
}

template <typename Config>
void handleLimitExceptDirective(Config &config, Node &node, std::string &directive)
{
    if (!config.getLimitExcept().empty())
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);

    for (std::size_t i = 0; i < node.args.size(); i++)
        config.pushBackLimitExcept(node.args.at(i));

    std::vector<std::string> limitExcept = config.getLimitExcept();
    std::set<std::string> seen;

    for (size_t i = 0; i < limitExcept.size(); ++i)
    {
        if (limitExcept.at(i) != "GET" && limitExcept.at(i) != "POST" && limitExcept.at(i) != "DELETE")
            throwInvalidLimitExceptValue(limitExcept.at(i), config.getFileName(), node.line);

        if (seen.count(limitExcept.at(i)))
            throwDuplicateValues(directive, node.args.at(0), config.getFileName(), node.line);
        seen.insert(limitExcept.at(i));
    }
}
