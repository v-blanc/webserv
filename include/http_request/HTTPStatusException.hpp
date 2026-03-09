#ifndef HTTPSTATUSEXCEPTION_HPP
#define HTTPSTATUSEXCEPTION_HPP

#include <exception>
#include <string>

class	HttpStatusException : public std::exception

{
	private:
		std::string _status;
		std::string _message;

	public:
		HttpStatusException(const std::string &status, const std::string &message)
							: _status(status), _message(message) {}
		virtual				~HttpStatusException() throw() {}
		virtual const char	*what() const throw() { return _message.c_str(); }
		const std::string	&getStatus() const { return _status; }
		const std::string 	&getMessage() const { return _message; }
};

struct CgiRequestInfo

{
		std::string 	interpreter;
		std::string 	method;
		std::string 	queryString;
		std::string 	pathWithoutQuery;
		unsigned long	contentLength;
		std::string 	contentType;
		std::string 	body;
		std::string		root;
};

class	CgiRequiredException : public std::exception

{
	private:
		CgiRequestInfo _cgiInfo;
	public:
		CgiRequiredException(const CgiRequestInfo &info) : _cgiInfo(info) {}
		virtual 			~CgiRequiredException() throw() {}
		virtual const char	*what() const throw() { return ("CGI execution required"); }
		const 				CgiRequestInfo &getCgiInfo() const { return _cgiInfo; }
};

#endif
