/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 12:10:43 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/06 12:53:58 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "colors.h"
#include "configStructs.h"
#include <ctime>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


// parseConfErrorMessage.cpp
void throwUnexpectedType(Token::Type type, int line, std::string &fileName);
void throwUnexpectedEOF(int line, std::string &fileName);
void throwSafeOpenFileError(std::ifstream &file, std::string &fileName);
void throwInvalidNumberOfArguments(std::string directive, std::string fileName, std::string line);

#endif
