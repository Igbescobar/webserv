#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include "../inc/request/HttpRequest.hpp"
#include <string>
#include <iostream>
class	CgiHandler
{
	private:
		HttpRequest &request;
		LocationConfig &location;
		std::string scriptPath;
		char	**buildEnv();
		char	**builkdArgv();
		void	setupChildProcess(int pipefd[2]);
	public:
		CgiHandler(HttpRequest &request, LocationConfig &location);
		~CgiHandler();
		std::string execute();
};
#endif
