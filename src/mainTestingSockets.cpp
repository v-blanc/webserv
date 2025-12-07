/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainTestingSockets.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 18:14:56 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/07 19:24:38 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"
#include "HTTPRequest.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>    // pour close()
#include <arpa/inet.h> // pour htons, htonl
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Usage: ./webser configuration" DEFAULT << std::endl;
        return (ERROR);
    }

    (void)argv;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Serveur prêt, en attente de connexion...\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("accept");
        return 1;
    }

    printf("Client connecté !\n");

    bool connection = 1;
    while (connection)
    {

        char buffer[4096];
        int n = read(client_fd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = '\0';
            printf("Requête reçue :\n%s\n", buffer);
        }

        std::string request;
        int i = 0;
        while (buffer[i])
            request.push_back(buffer[i++]);

        HTTPRequest httpRequest(request);
        printHTTPRequest(httpRequest);
        connection = httpRequest.getConnection();
    }

    close(client_fd);
    close(server_fd);

    return (SUCCESS);
}
