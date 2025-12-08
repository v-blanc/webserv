/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:35:58 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 13:15:25 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_H
#define DEFINE_H

// LIBS
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <netdb.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// RETURN VALUES
#define SUCCESS 0
#define ERROR 1

// TYPEDEF
typedef std::pair<std::string, std::string> stringPair;
typedef std::pair<uint32_t, uint16_t> listenPair;

#endif
