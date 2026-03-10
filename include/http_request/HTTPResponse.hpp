/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vblanc <vblanc@student.42lyon.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:49:54 by yassinefahf       #+#    #+#             */
/*   Updated: 2026/03/10 01:32:14 by vblanc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#include "HTTPRequest.hpp"
#include "SessionManager.hpp"
#include <sys/stat.h>
#include <sys/wait.h>

class HTTPRequest;
class HTTPResponse
{
private:
	ServerConfig &_serverConfig;
	SessionManager &_sessionManager;
	std::string _body;
	std::size_t _contentLength;
	std::string _response;
	std::string _status;
	std::string _message;
	std::string _newLocation;
	std::string _sessionId;
	int _redirectCount;

public:
	HTTPResponse(HTTPRequest &request, const std::string &status, ServerConfig &serverConfig, std::string message, SessionManager &sessionManager);
	~HTTPResponse();
	void handleGetMethod(HTTPRequest &request);
	void handleBadRequest(const std::string &status, const std::string &message);
	void handlePostMethod(HTTPRequest &request);
	void handleDeleteMethod(HTTPRequest &request);

	static bool ismethodNotAllowed(std::vector<std::string> methods, std::string myMethod);
	void handleIndexFile(const LocationConfig &myLocation, std::string &path);
	const std::string &getResponse() const { return this->_response; }
	void prepareGoodResponse();
	std::string handleRequestPath(std::string requestPath, bool isFileName);
	void handleRessource(HTTPRequest &request);
	void fillLocationWithServerRules(LocationConfig &location);
	std::string resolveRoot(const std::string &root);
};
#endif
