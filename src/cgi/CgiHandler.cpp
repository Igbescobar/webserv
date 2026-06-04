#include <unistd.h>
#include <sys/wait.h>
#include "../inc/cgi/CgiHandler.hpp"
#include "../inc/parser/config/LocationConfig.hpp"
#define BUF_SIZE 4096

CgiHandler::CgiHandler(HttpRequest &request, LocationConfig &location): request(request), location(location)
{}

CgiHandler::~CgiHandler()
{}

/*char**	CgiHandler::buildEnv()
{

}*/

std::string CgiHandler::getInterpreter(const std::string &ext)
{
    if (ext == ".py")
        return "/usr/bin/python3";
    if (ext == ".php")
        return "/usr/bin/php-cgi";
    if (ext == ".sh")
        return "/bin/bash";
    return "";
}

std::string	CgiHandler::execute()
{
	//LocationConfig location;
	//TODO:start studying of cgi execution
	//TODO:search child proccesses and pipes
	//TODO:make sure it executes with different files

	std::string uri = request.getUri();
	//Getting filename
	size_t filePosition = uri.find_last_of('/');
	if(filePosition == std::string::npos)
	{
		std::cout<<"Does not have a correct file name";
		return "";
	}
	std::string scriptPath = uri.substr(filePosition);
	std::cout<<scriptPath<<"\n";
	//Getting extension
	size_t dotPos = uri.find_last_of('.');
	if(dotPos == std::string::npos)
	{
		std::cout<<"Does not have a correct file extension";
		return "";
	}
	std::string extensionPath = uri.substr(dotPos);
	std::cout<<extensionPath<<"\n";
	std::string interpreter =getInterpreter(extensionPath);
	//Build argv
	char *argv[] = 
	{
		const_cast<char *>(interpreter.c_str()),
		const_cast<char *>(scriptPath.c_str()),
		NULL
	};
	//Build env
	std::vector<std::string> env;
	//Build qurry string
	size_t queryPos = uri.find('?');
	std::string query = (queryPos != std::string::npos) ? uri.substr(queryPos + 1) : "";
	env.push_back("REQUEST_METHOD=" + request.getMethod());
	env.push_back("CONTENT_LENGTH=" + request.getHeader("content-length"));
	env.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));
	env.push_back("QUERY_STRING=" + query);
	std::cout<<env[0]<<"\n";
	std::vector<char *> envp;
	for(size_t i = 0; i < env.size(); i++)
	{
		envp.push_back(const_cast<char *>(env[i].c_str()));
	}
	envp.push_back(NULL);
	//Execute
	int	stdinpipe[2];
	int	stdoutpipe[2];
	pipe(stdinpipe);
	pipe(stdoutpipe);

	pid_t pid = fork();
	if (pid == 0)
	{
		//hijo (CGI)
		dup2(stdinpipe[0], STDIN_FILENO);
		dup2(stdoutpipe[1], STDOUT_FILENO);
		close(stdinpipe[1]);
		close(stdoutpipe[0]);
		execve(interpreter.c_str(), argv, envp.data());
		exit(1);
	}
	else
	{
		close(stdinpipe[0]);
		close(stdoutpipe[1]);
		std::string body = request.getBody();
		write(stdinpipe[1], body.c_str(), body.size());
		close(stdinpipe[1]);
		waitpid(pid, NULL, 0);
		std::string output;
		char buf[BUF_SIZE];
		int bytes;
		while ((bytes = ::read(stdoutpipe[0], buf, BUF_SIZE)) > 0)
			output += std::string(buf, bytes);
		close(stdoutpipe[0]);
		return output;
	}
	return scriptPath;
}
	/*int	stdinpipe[2];
	int	stdoutpipe[2];
	pipe(stdinpipe);
	pipe(stdoutpipe);

	pid_t pid = fork();
	if (pid == 0)
	{
		//hijo (CGI)
		dup2(stdinpipe[0], STDIN_FILENO);
		dup2(stdoutpipe[1], STDOUT_FILENO);
		close(stdinpipe[1]);
		close(stdoutpipe[0]);
		execve(scripPath, argv, envp);
	}
	else
	{
		close(stdinpipe[0]);
		close(stdoutpipe[1]);
		write(stdinpipe[1], body.c_str(), body.size());
		close(stdinpipe[1]);
		waitpid(pid, NULL, 0);
	}*/
