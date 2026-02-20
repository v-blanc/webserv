#ifndef CGI_HPP
#define CGI_HPP

#include "utils.hpp"
#include "structServer.h"
#include "HTTPStatusException.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

std::vector<std::string>	buildCgiEnv(CgiRequestInfo const& cgiInfo, std::string const& scriptFilename);
char**						vectorToEnvp(std::vector<std::string> const& env);
void						freeEnvp(char** envp);

CgiContext*	executeCgi(
	CgiRequestInfo const& cgiInfo,
	ClientContext* clientContext,
	int epfd
);

#endif

