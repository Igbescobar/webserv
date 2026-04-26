#include <iostream>
#include "HttpRequest.hpp"

//HttpRequest::HttpRequest(): method(method), uri(uri), version(version), headers(headers), body(body)
//{}

HttpRequest::HttpRequest(const std::string &rawData)
{
	//std::cout<<"HtttpRequest constructor called"<<std::endl;
	parseRawData(rawData);
}

HttpRequest::~HttpRequest()
{
	//std::cout<<"Http Destructor called"<<std::endl;
}

std::string	extractString(std::string Data, std::string delimeter)
{
	size_t	stringEnd = Data.find(delimeter);
	if(stringEnd == std::string::npos)
		std::cout<<"Error, stringEnd not found"<<std::endl;

	std::string result = Data.substr(0, stringEnd);

	return result;
}

void	HttpRequest::initializeFirstLineValues(std::string header)
{
	size_t firstLineFirstSpace = header.find(" ");
	size_t firstLineSecondSpace = header.find(" ", firstLineFirstSpace + 1);

	this->method = header.substr(0, firstLineFirstSpace);
	this->uri = header.substr(firstLineFirstSpace + 1, firstLineSecondSpace - firstLineFirstSpace);
	this->version = header.substr(firstLineSecondSpace + 1);
}


void	HttpRequest::parseHeaderLine(const std::string headerLine)
{
	std::string line = header.substr(pos, lineEnd - pos);
	size_t colonPos = line.find(":");
	std::string key = line.substr(0, colonPos);
	std::string value = line.substr(colonPos + 2, lineEnd - (colonPos + 2));
	this->headers[key] = value;
}
void	HttpRequest::parseHeaders(const std::string header, size_t pos)
{

	while(pos < header.length())
	{
		size_t lineEnd = header.find("\r\n", pos);
		if(lineEnd == std::string::npos)
			lineEnd = header.length();
		parseHeaderLine(header.substr(pos, lineEnd - pos));
		pos = lineEnd + 2;
	}
}
void	HttpRequest::parseRawData(const std::string &rawData)
{
	std::string	header = extractString(rawData, "\r\n\r\n");
	std::string	requestLine = extractString(header, "\r\n");

	initializeFirstLineValues(requestLine);
	parseHeaders(header, header.find("\r\n") + 2);
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
