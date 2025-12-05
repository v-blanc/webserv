/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:30:16 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 12:14:33 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
#define CONFIG_H

#include "Lexer.hpp"

// Attributes and methods can be renamed or removed, and params can be changed

class Config
{
public:
    Config(const char *fileName);
    ~Config();

private:
    Lexer _lexer;
};

#endif
