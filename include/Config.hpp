/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:30:16 by vblanc            #+#    #+#             */
/*   Updated: 2025/11/25 15:13:37 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>

// Attributes and methods can be renamed or removed, and params can be changed

class Config
{
public:
    Config();
    Config(std::string fileName);
    ~Config();

private:
    std::string _fileName;

    void openFile(void) const;
    void closeFile(void) const;
};

#endif
