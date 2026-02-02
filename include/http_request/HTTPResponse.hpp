/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:49:54 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/02/02 14:51:30 by yassinefahf      ###   ########.fr       */
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
		void handleIndexFile(const LocationConfig &myLocation);
		void handleAutoIndex(const LocationConfig &myLocation);
		const std::string &getResponse() const {return this->_response;}
		void prepareGoodResponse();
};
#endif