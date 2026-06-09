#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include "../inc/request/HttpRequest.hpp"
#include <string>
#include <iostream>

class	CgiHandler
{
	private:
		HttpRequest &request;
		const LocationConfig &location;
		std::string scriptPath;
		std::string interpreter;
		std::string query;

		std::string getInterpreter(const std::string &ext);
		std::string extractScriptPath();
		std::string extractExtension();
		std::string extractQuery();
		std::vector<std::string> buildEnv();
		std::string buildResponse(std::string &output, int stdoutpipe[2]);
		void	setupChild(int stdinpipe[2],int stdoutpipe[2], char *argv[], char **envp);
	public:
		CgiHandler(HttpRequest &request, const LocationConfig &location);
		~CgiHandler();
		std::string execute();
};
#endif
