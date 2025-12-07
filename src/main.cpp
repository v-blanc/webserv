/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:57:33 by vblanc            #+#    #+#             */
/*   Updated: 2025/12/07 17:02:57 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GlobalConfig.hpp"
#include "HTTPRequest.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED "Usage: ./webser configuration" DEFAULT << std::endl;
        return (ERROR);
    }

    (void)argv;
    GlobalConfig globalConfig(argv[1]);
    printGlobalConfig(globalConfig);

    std::string request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\nContent-Length: 219\r\n\r\n";
    request += "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\r\n";
    request += "<TITLE>301 Moved</TITLE></HEAD><BODY>\r\n<H1>301 Moved</H1>\r\nThe document has moved\r\n";
    request += "<A HREF=\"http://www.google.com/\">here</A>.\r\n</BODY></HTML>\r\n";

    HTTPRequest httpRequest(request);
    printHTTPRequest(httpRequest);

    return (SUCCESS);
}
