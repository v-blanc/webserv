/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveHandler.tpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 14:21:38 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 17:48:50 by vblanc           ###   ########.fr       */
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
    else
    {
        if (node.args.at(0) == "on")
            config.setAutoindex(true);
        else if (node.args.at(0) == "off")
            config.setAutoindex(false);
        else
            throwInvalidAutoindexValue(node.args.at(0), config.getFileName(), node.line);
    }
}

template <typename Config>
void handleClientMaxBodySize(Config &config, Node &node, std::string &directive)
{
    if (config.getClientMaxBodySize() != -1)
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);
    else
    {
        std::stringstream ss(node.args.at(0));
        long long result;
        ss >> result;
        if (ss.fail() || result < 0 || node.args.at(0).size() != to_string(result).size())
            throwInvalidClientMaxValue(directive, config.getFileName(), node.line);
        config.setClientMaxBodySize(result);
    }
}

template <typename Config>
void handleRootDirective(Config &config, Node &node, std::string &directive)
{
    if (!config.getRoot().empty())
        throwDirectiveIsDuplicate(directive, config.getFileName(), node.line);
    else if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);
    else
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
    else
        for (std::size_t i = 0; i < node.args.size(); i++)
            config.pushBackErrorPage(node.args.at(i));
}

template <typename Config>
void handleListenDirective(Config &config, Node &node, std::string &directive)
{
    if (node.args.size() != 1)
        throwInvalidNumberOfArguments(directive, config.getFileName(), node.line);
    else
    {
        config.pushBackListen(node.args.at(0));

        std::vector<std::string> listen = config.getListen();
        std::set<std::string> seen;

        for (size_t i = 0; i < listen.size(); ++i)
        {
            if (seen.count(listen.at(i)))
                throwDuplicateValues(directive, node.args.at(0), config.getFileName(), node.line);
            seen.insert(listen.at(i));
        }
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
    else
    {
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
}
