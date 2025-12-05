/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/05 20:01:50 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Usage: ./webser configuration" DEFAULT << std::endl;
        return (ERROR);
    }

    GlobalConfig globalConfig(argv[1]);

    return (SUCCESS);
}
