/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassinefahfouhi <yassinefahfouhi@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:49:54 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/02/12 13:13:15 by yabokhar         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#include "HTTPRequest.hpp"
#include "SessionManager.hpp"

class HTTPRequest;
class HTTPResponse
{
	private:
		ServerConfig &_serverConfig;
		SessionManager &_sessionManager;
		std::string	_body;
		std::size_t _contentLength;
		std::string _response;
		std::string _status;
		std::string	_message;
		std::string	_newLocation;
		std::string	_sessionId;

	public:
		HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message, SessionManager &sessionManager);
		~HTTPResponse();
		void handleGetMethod(HTTPRequest &request);
		void handleBadRequest(const std::string &status, const std::string &message);
		void handlePostMethod(const HTTPRequest &request);
		void handleDeleteMethod(HTTPRequest &request);

		static bool ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod);
		void handleIndexFile(const LocationConfig &myLocation);
		const std::string &getResponse() const {return this->_response;}
		void prepareGoodResponse();
		std::string	handleRequestPath(std::string requestPath, bool isFileName);
		void handleRessource(HTTPRequest &request);
};
#endif
