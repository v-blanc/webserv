/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structServer.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:24:29 by vblanc            #+#    #+#             */
/*   Updated: 2026/03/09 17:16:47 by vblanc           ###   ########.fr       */
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

enum ClientState
{
    READING_HEADERS,
    READING_BODY,
    READY_TO_SEND,
    SENDING
};

struct ClientContext : EpollContext
{
    ClientState state;

    // Request params
    std::string recvBuffer;
    bool isChunkedRequest;
    std::size_t currentUnchunkedIndex;
    std::size_t expectedBodySize;
    std::size_t bodyStartIndex;

    // Response params
    std::string sendBuffer;
    ssize_t sendBufferIndex;

    // Client info
    int serverFd;
    bool keepAlive;
    time_t lastActive;
};

struct CgiContext : EpollContext
{
    ClientContext *client;
    pid_t pid;
    time_t startTime;
    std::string script;
    std::string output;
};

#endif
