/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structServer.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:24:29 by vblanc            #+#    #+#             */
/*   Updated: 2026/01/17 22:12:07 by vblanc           ###   ########.fr       */
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
    // Request params
    std::string recvBuffer;
    bool isChunkedRequest;
    bool headerIsComplete;
    std::size_t currentUnchunkedIndex;
    std::size_t expectedBodySize;
    std::size_t bodyStartIndex;
    bool requestIsComplete;

    // Response params
    std::string sendBuffer;
    std::size_t sendBufferIndex;

    // Client info
    int serverFd;
    bool keepAlive;
    time_t lastActive;
};

#endif
