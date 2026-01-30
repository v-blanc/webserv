/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:49:54 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/01/27 19:46:03 by yassinefahf      ###   ########.fr       */
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
		std::string	_body;
		std::size_t _contentLength;
		std::string _response;
	public:
		HTTPResponse(const HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message);
		~HTTPResponse();
		void handleGetMethod(const HTTPRequest &request);
		void handleBadRequest(const std::string &status, const std::string &message);
		void handlePostMethod(const HTTPRequest &request);
		static bool ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod);
		
		const std::string &getResponse() const {return this->_response;}
		void prepareResponse(const HTTPRequest &request);
};
#endif