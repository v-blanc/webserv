/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structServer.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:24:29 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/19 13:41:25 by vblanc           ###   ########.fr       */
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
    // receiveBuffer;
    // sendBuffer;
    // parseState;
    time_t  lastActive;
    bool    keepAlive;
    int     serverFd;
};

#endif
