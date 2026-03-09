/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 12:35:58 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/09 18:53:02 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_H
#define DEFINE_H

// LIBS
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

// DEFINE
#define SUCCESS 0
#define ERROR 1

#define DEFAULT_CONFIG_PATH "config/good/webserv.conf"
#define MAX_EPOLL_WAIT_EVENTS 128
#define RECV_BUFFER_SIZE 4096
#define MAX_HEADER_SIZE 88192
#define TIMEOUT_OLD_CONNECTIONS 20 // in seconds
#define CGI_TIMEOUT 10

// TYPEDEF
typedef std::pair<std::string, std::string> stringPair;
typedef std::pair<uint32_t, uint16_t> listenPair;

#endif
