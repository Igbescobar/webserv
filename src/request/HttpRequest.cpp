#include "request/HttpRequest.hpp"
#include "parser/config/ServerConfig.hpp"
#include <string>

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(ServerConfig serverConfig)
    : serverConfig(serverConfig) {
  errorCode = -1;
  state = INCOMPLETE;
}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  return *this;
}

void HttpRequest::append(std::string chunk) {
  buf += chunk;
  updateState();
}

void HttpRequest::updateState() {
  size_t pos = buf.find(DELIMETER);
  std::cout<<"VALOR DE BUF EN EL MOMENTO DE LA COMPROBACION DE RNRN: "<<buf<<"\n";
  std::cout<<"VALOR DE POS EN EL MOEMNTO DE LA COMPROBACIOND DE RNRN: "<<pos<<"\n";
  if (pos == std::string::npos) {
    state = INCOMPLETE;
    return;
  }
  state = COMPLETE;
  printState();
}

void	HttpRequest::checkFind(size_t parameter)
{
	if(parameter == std::string::npos)
	{
		std::cout<<"AQUIIIIIIIIIII\n";
		state = ERROR;
	}
}

void	HttpRequest::checkRequestLine()
{
	if(method != "GET" && method != "POST" && method != "DELETE" && method != "HEAD")
		state = ERROR;
	if(version != "HTTP/1.1")
		state = ERROR;
}

void	HttpRequest::checkRequestHeaders()
{
	if (getHeader("host").empty())
	{
		std::cout<<"AQUI\n";
		state = ERROR;
	}
	const std::string &cl = getHeader("content-length");
	std::cout<<"CL : "<<cl<<"\n";
	for (size_t i = 0; i < cl.size(); i++)
		if (!isdigit(cl[i]))
		{
			std::cout<<"CL[i] "<<cl[i]<<"\n";
			std::cout<<"AQUI1\n";
			state = ERROR;
		}
}

const std::string	HttpRequest::extractString(const std::string &Data,const std::string &delimeter)
{
	size_t	stringEnd = Data.find(delimeter);
	checkFind(stringEnd);
	return Data.substr(0, stringEnd);
}

void	HttpRequest::parseFirstLineValues(const std::string &requestLine)
{
	size_t firstLineFirstSpace = requestLine.find(" ");
	checkFind(firstLineFirstSpace);
	size_t firstLineSecondSpace = requestLine.find(" ", firstLineFirstSpace + 1);
	checkFind(firstLineSecondSpace);
	this->method = requestLine.substr(0, firstLineFirstSpace);
	this->uri = requestLine.substr(firstLineFirstSpace + 1, firstLineSecondSpace - firstLineFirstSpace - 1);
	this->version = requestLine.substr(firstLineSecondSpace + 1);
	checkRequestLine();
}

void	HttpRequest::parseHeaderLine(const std::string &headerLine)
{
	if(headerLine == "\r\n" || headerLine.empty())
		return;
	size_t colonPos = headerLine.find(":");
	std::string key = headerLine.substr(0, colonPos);
	if(key.empty() || key.find(' ') != std::string::npos || key.find('\t') != std::string::npos)
	{
		std::cout<<"VACIO O CON ESPACIOS\n";
		state = ERROR;
	}
	for(size_t i = 0; i < key.size(); i++)
		key[i] = tolower(key[i]);
	size_t valueStart = headerLine.find_first_not_of(" \t", colonPos + 1);
	std::string value = headerLine.substr(valueStart);
	size_t end = value.find_last_not_of("\r\n");
	if(end != std::string::npos)
		value = value.substr(0, end + 1);
	this->headers[key] = value;
	std::cout<<"KEY : "<<key<<"\n VALUE :"<<value<<"\n";
	checkRequestHeaders();
}

void	HttpRequest::parseHeaders(const std::string &header, size_t pos)
{
	if(pos > header.length())
		state = ERROR;
	while(pos < header.length())
	{
		size_t lineEnd = header.find("\r\n", pos);
		if(lineEnd == std::string::npos)
			lineEnd = header.length();
		std::cout<<"----------------------------\n";
		parseHeaderLine(header.substr(pos, (lineEnd + 2) - pos));
		pos = lineEnd + 2;
	}
}

void HttpRequest::parseBody(const std::string &rawData)
{
	size_t bodyStart = rawData.find(DELIMETER);
	bodyStart += 4;
	this->body = rawData.substr(bodyStart);
}

void	HttpRequest::checkIfLineComplete(std::string chunk)
{
	append(chunk);
	size_t endLine = buf.find("\r\n");
	if(endLine == std::string::npos)
		return;
	std::cout<<"CHUNK: "<<chunk<<"\n";
	std::cout<<"BUF: "<<buf<<"\n";
	size_t pos = 0;
	std::string headerLine = buf.substr(pos, endLine);
	parseFirstLineValues(headerLine);
	pos = endLine + 2;
	parseHeaders(buf, pos);
}

void	HttpRequest::printState()
{
	if(state == INCOMPLETE)
		std::cout<<"INCOMPLETE\n";
	else if(state == COMPLETE)
		std::cout<<"COMPLETE\n";
	else
		std::cout<<"ERROR\n";
}

void	HttpRequest::parseRawData(std::string chunk)
{
	//TODO Subsitute rawData logic for buffer logic
	std::cout<<"------------------------------------------\n";
	checkIfLineComplete(chunk);
}

std::string	HttpRequest::getMethod() const
{
	return this->method;
}
std::string	HttpRequest::getUri() const
{
	return this->uri;
}

std::string	HttpRequest::getVersion() const
{
	return this->version;
}

std::string	HttpRequest::getBody() const
{
	return	this->body;
}

std::string	HttpRequest::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it == headers.end())
	{
		return "";
	}
	return it->second;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const
{
	return this->headers;
}

ServerConfig HttpRequest::getServerConfig() { return serverConfig; }

t_state HttpRequest::getState() { return state; }

int HttpRequest::getErrorCode() { return errorCode; }

std::string HttpRequest::getRequest() { return buf; }
