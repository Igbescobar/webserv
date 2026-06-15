#include <unistd.h>
#include <sys/wait.h>
#include "../inc/cgi/CgiHandler.hpp"
#include "../inc/parser/config/LocationConfig.hpp"
#include <sstream>
#define BUF_SIZE 4096

CgiHandler::CgiHandler(HttpRequest &request, const LocationConfig &location): request(request), location(location)
{}

CgiHandler::~CgiHandler()
{}

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

std::string	CgiHandler::extractScriptPath()
{
	size_t filePosition = request.getUri().find_last_of('/');
	if(filePosition == std::string::npos)
	{
		std::cout<<"Does not have a correct file name";
		return "";
	}
	size_t queryPos = request.getUri().find('?', filePosition);
	std::string fileName = (queryPos != std::string::npos) 
	? request.getUri().substr(filePosition, queryPos - filePosition)
	: request.getUri().substr(filePosition);
	fileName.erase(0,1);
	return  location.getRoot() + fileName;
}

std::string	CgiHandler::extractExtension()
{
	size_t dotPos = request.getUri().find_last_of('.');
	if(dotPos == std::string::npos)
	{
		std::cout<<"Does not have a correct file extension";
		return "";
	}
	size_t questionpos = request.getUri().find('?', dotPos);
	std::string extensionPath = (questionpos != std::string::npos) 
		? request.getUri().substr(dotPos, questionpos - dotPos)
		: request.getUri().substr(dotPos);
	std::cout<<"Extesion path:"<<extensionPath<<"\n";
	return extensionPath;
}

std::string CgiHandler::extractQuery()
{
    size_t queryPos = request.getUri().find('?');
    return (queryPos != std::string::npos) ? request.getUri().substr(queryPos + 1) : "";
}

std::vector<std::string> CgiHandler::buildEnv()
{
	std::vector<std::string> env;
	env.push_back("REQUEST_METHOD=" + request.getMethod());
	env.push_back("CONTENT_LENGTH=" + request.getHeader("content-length"));
	env.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));
	env.push_back("QUERY_STRING=" + query);
	return env;
}

void	CgiHandler::setupChild(int stdinpipe[2],int stdoutpipe[2], char *argv[], char **envp)
{
	std::string scriptDir = scriptPath.substr(0, scriptPath.find_last_of('/'));
	chdir(scriptDir.c_str());
	dup2(stdinpipe[0], STDIN_FILENO);
	dup2(stdoutpipe[1], STDOUT_FILENO);
	close(stdinpipe[1]);
	close(stdoutpipe[0]);
	execve(interpreter.c_str(), argv, envp);
	exit(1);
}

std::string	CgiHandler::readPipe(int stdoutpipe[2])
{
	std::string output;
	char buf[BUF_SIZE];
	int bytes;
	while ((bytes = ::read(stdoutpipe[0], buf, BUF_SIZE)) > 0)
		output += std::string(buf, bytes);
	close(stdoutpipe[0]);
	return output;
}

std::string	CgiHandler::buildResponse(std::string &output)
{
	size_t sepPos = output.find("\r\n\r\n");
	std::string cgiHeaders;
	std::string cgiBody;
	if (sepPos != std::string::npos)
	{
		cgiHeaders = output.substr(0, sepPos);
		cgiBody = output.substr(sepPos + 4);
	}
	else
		cgiBody = output;
	std::ostringstream response;
	response << "HTTP/1.1 200 OK\r\n";
	response << cgiHeaders << "\r\n";
	if (cgiHeaders.find("Content-Length") == std::string::npos)
		response << "Content-Length: " << cgiBody.size() << "\r\n";
	response<<"\r\n";
	response << cgiBody;
	return response.str();
}

std::string	CgiHandler::execute()
{
	scriptPath = extractScriptPath();
	interpreter =getInterpreter(extractExtension());
	query = extractQuery();
	char *argv[] = 
	{
		const_cast<char *>(interpreter.c_str()),
		const_cast<char *>(scriptPath.c_str()),
		NULL
	};
	//Build env
	std::vector<std::string> env = buildEnv();
	//Build query string
	std::vector<char *> envp;
	std::cout<<"ENV: "<<"\n";
	for(size_t i = 0; i < env.size(); i++)
	{
		std::cout<<env[i]<<"\n";
		envp.push_back(const_cast<char *>(env[i].c_str()));
	}
	envp.push_back(NULL);
	//Execute
	int	stdinpipe[2];
	int	stdoutpipe[2];
	
	if(pipe(stdinpipe) < 0 || pipe(stdoutpipe) < 0)
		return "";

	pid_t pid = fork();
	if(pid < 0)
		return "";
	if (pid == 0)
		setupChild(stdinpipe, stdoutpipe, argv, envp.data());
	else
	{
		close(stdinpipe[0]);
		close(stdoutpipe[1]);
		std::string body = request.getBody();
		write(stdinpipe[1], body.c_str(), body.size());
		close(stdinpipe[1]);
		waitpid(pid, NULL, 0);
		std::string output = readPipe(stdoutpipe);
                return buildResponse(output);
	 }
	return "";
}
