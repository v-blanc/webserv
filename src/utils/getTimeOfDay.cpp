/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getTimeOfDay.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 14:24:20 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/08 14:24:40 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "define.h"

std::string getTimeOfDay()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", t);

    return (buffer);
}
