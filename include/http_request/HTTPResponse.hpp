/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:49:54 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/02/09 18:43:21 by yassinefahf      ###   ########.fr       */
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
		std::string _status;
		std::string	_message;
		std::string	_newLocation;

	public:
		HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message);
		~HTTPResponse();
		void handleGetMethod(HTTPRequest &request);
		void handleBadRequest(const std::string &status, const std::string &message);
		void handlePostMethod(const HTTPRequest &request);
		static bool ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod);
		void handleIndexFile(const LocationConfig &myLocation);
		const std::string &getResponse() const {return this->_response;}
		void prepareGoodResponse();
		std::string	handleRequestPath(std::string requestPath, bool isFileName);
};
#endif