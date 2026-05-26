#include "../inc/cgi/CgiHandler.hpp"
CgiHandler::CgiHandler(HttpRequest &request, LocationConfig &location): request(request), location(location)
{}

CgiHandler::~CgiHandler()
{}

/*char**	CgiHandler::buildEnv()
{

}*/
std::string	CgiHandler::execute()
{
	//TODO:start studying of cgi execution
	//TODO:search child proccesses and pipes
	//TODO:make sure it executes with different files

	this->request.getMethod()

	char *envp[] = {
		"REQUEST_METHOD=POST",
		"CONTENT_LENGHT=16",
		"QUERY_STRNG=",
		NULL
	};
	execve(script_path, argv, envp)
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
