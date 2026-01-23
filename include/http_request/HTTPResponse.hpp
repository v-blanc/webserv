/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:49:54 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/01/21 16:33:58 by yassinefahf      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#include "HTTPRequest.hpp"

class HTTPRequest;
class HTTPResponse
{
	private:
		ServerConfig &_serverConfig;
	public:
		HTTPResponse(const HTTPRequest &request, const std::string &status, ServerConfig &serverConfig);
		~HTTPResponse();
		void handleGetMethod(const HTTPRequest &request);
		void handleBadRequest(const std::string &status);
		void handlePostMethod(const HTTPRequest &request);
};
#endif