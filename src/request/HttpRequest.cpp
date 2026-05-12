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
  if (pos == std::string::npos) {
    state = INCOMPLETE;
    return;
  }
  state = COMPLETE;
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
		state = ERROR;
	const std::string &cl = getHeader("content-length");
	for (size_t i = 0; i < cl.size(); i++)
		if (!isdigit(cl[i]))
			state = ERROR;
}

void	HttpRequest::parseRequestLineValues(const std::string &requestLine)
{
	size_t first = requestLine.find(" ");
	if (first == std::string::npos) { state = ERROR; return; }
	size_t second = requestLine.find(" ", first + 1);
	if (second == std::string::npos) { state = ERROR; return; }
	method = requestLine.substr(0, first);
	uri = requestLine.substr(first + 1, second - first - 1);
	version = requestLine.substr(second + 1);
	checkRequestLine();
}

void	HttpRequest::parseHeaderLine(const std::string &headerLine)
{
	if(headerLine == "\r\n" || headerLine.empty())
		return;
	size_t colonPos = headerLine.find(":");
	if(colonPos == std::string::npos)
		return;
	std::string key = headerLine.substr(0, colonPos);
	if(key.empty() || key.find(' ') != std::string::npos || key.find('\t') != std::string::npos)
		state = ERROR;
	for(size_t i = 0; i < key.size(); i++)
		key[i] = tolower(key[i]);
	size_t valueStart = headerLine.find_first_not_of(" \t", colonPos + 1);
	std::string value = headerLine.substr(valueStart);
	size_t end = value.find_last_not_of("\r\n");
	if(end != std::string::npos)
		value = value.substr(0, end + 1);
	this->headers[key] = value;
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
		std::cout<<"\033[31m"<<"----------------------------\n"<<"\033[0m";
		parseHeaderLine(header.substr(pos, (lineEnd + 2) - pos));
		pos = lineEnd + 2;
	}
	checkRequestHeaders();
}

void HttpRequest::parseBody(const std::string &rawData)
{
	size_t bodyStart = rawData.find(DELIMETER);
	bodyStart += 4;
	this->body = rawData.substr(bodyStart);
}


void HttpRequest::parseRequestLine()
{
    size_t end = buf.find("\r\n");
    if (end == std::string::npos) { state = ERROR; return; }
    parseRequestLineValues(buf.substr(0, end));
    headerStart = end + 2; // guarda la posición para parseHeaders
}

void	HttpRequest::parse(std::string chunk)
{
	std::cout<<"\033[32m"<<"------------------------------------------\n"<<"\033[0m";
	append(chunk);
	if(state != COMPLETE)
		return;
	parseRequestLine();
	if(state == ERROR)
		return;
	parseHeaders(buf, headerStart);
	if(state == ERROR)
		return;
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
