/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structServer.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:24:29 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/11 19:23:05 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTSERVER_H
#define STRUCTSERVER_H

#include "define.h"

struct EpollContext
{
    int fd;
    virtual ~EpollContext() {}
};

struct ServerContext : EpollContext
{
};

struct ClientContext : EpollContext
{
    // Request
    std::string recvBuffer;
    std::size_t expectedBodySize;
    bool isChunkedRequest;
    bool headerIsComplete;
    std::size_t bodyStartIndex;
    bool requestIsComplete;

    // Response
    std::string sendBuffer;
    std::size_t sendBufferIndex;

    // Client info
    int serverFd;
    time_t lastActive;
    bool keepAlive;
};

#endif
