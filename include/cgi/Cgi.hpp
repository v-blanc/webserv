#ifndef CGI_HPP
#define CGI_HPP

#include "utils.hpp"
#include "HTTPRequest.hpp"
#include "structServer.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

std::vector<std::string>	buildCgiEnv(HTTPRequest const& req, std::string const& scriptFilename);
char**						vectorToEnvp(std::vector<std::string> const& env);
void						freeEnvp(char** envp);

CgiContext*	launchCgi(
	HTTPRequest const& httpRequest,
	std::string const& interpreter,
	ClientContext* clientContext,
	int epfd
);

#endif

