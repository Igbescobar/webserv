#include "../inc/cgi/CgiHandler.hpp"
#include "../inc/parser/config/LocationConfig.hpp"

CgiHandler::CgiHandler(HttpRequest &request, LocationConfig &location): request(request), location(location)
{}

CgiHandler::~CgiHandler()
{}

/*char**	CgiHandler::buildEnv()
{

}*/
std::string	CgiHandler::execute()
{
	LocationConfig location;
	//TODO:start studying of cgi execution
	//TODO:search child proccesses and pipes
	//TODO:make sure it executes with different files

	std::vector<std::string> env;
	env.push_back("REQUEST_METHOD=" + request.getMethod());
	env.push_back("CONTENT_LENGTH=" + request.getHeader("content-length"));
	env.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));

	std::cout<<envp[0]<<"\n";
	std::vector<char *> envp;
	for(size_t i = 0; i < env.size(); i++)
	{
		envp.push_back(env[i].c_str());
	}
	envp.push_back(NULL);
	execve(scriptPath, argv, envp);
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
