/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structsServer.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:24:29 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/10 14:50:52 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTSERVER_H
#define STRUCTSERVER_H

#include "define.h"

struct ConnexionState
{
    int fd;
    // receiveBuffer;
    // sendBuffer;
    // parseState;
    time_t lastActive;
    bool keepAlive;
};

#endif
