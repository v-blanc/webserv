/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getLocalFileContent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 13:31:19 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/24 20:14:44 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string getLocalFileContent(const std::string &fileName)
{
    std::ifstream file(fileName.c_str());
    std::string content, buf;

    if (!file.is_open())
        throwSafeOpenFileError(file, fileName);

    while (std::getline(file, buf))
        content.append(buf + "\n");

    file.close();
    if (file.fail() && !file.eof())
    {
        std::cerr << RED + getTimeOfDay() + " [warning] : Error with ‘" ITALIC << fileName << DEFAULT RED "’ configuration file: \"" ITALIC;
        std::cerr << "The file couldn't be closed properly\"" DEFAULT << std::endl;
        return ("");
    }

    return (content);
}
