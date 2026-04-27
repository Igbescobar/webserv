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

void	checkFind(size_t parameter, std::string message, int code)
{
	if(parameter  ==  std::string::npos)
		throw HttpRequest::HttpRequestException(message, code);
}

void	HttpRequest::checkRequestLine()
{
	if(method != "GET" && method != "POST" && method != "DELETE")
		throw HttpRequest::HttpRequestException("Undefined method", 404);
	if(version != "HTTP/1.1")
		throw HttpRequest::HttpRequestException("HTML version not valid", 404);
}

std::string	extractString(const std::string &Data,const std::string &delimeter)
{
	size_t	stringEnd = Data.find(delimeter);
	if(stringEnd == std::string::npos)
		throw HttpRequest::HttpRequestException("Data  not correctly extracted", 400);

	const std::string &result = Data.substr(0, stringEnd);

	return result;
}

void	HttpRequest::initializeFirstLineValues(std::string &header)
{
	size_t firstLineFirstSpace = header.find(" ");

	checkFind(firstLineFirstSpace, "Bad formed request line", 400);
	size_t firstLineSecondSpace = header.find(" ", firstLineFirstSpace + 1);
	checkFind(firstLineSecondSpace, "Bad formed request line", 400);
	this->method = header.substr(0, firstLineFirstSpace);
	this->uri = header.substr(firstLineFirstSpace + 1, firstLineSecondSpace - firstLineFirstSpace - 1);
	this->version = header.substr(firstLineSecondSpace + 1);
	checkRequestLine();
}


void	HttpRequest::parseHeaderLine(const std::string &headerLine)
{
	size_t colonPos = headerLine.find(":");
	if(colonPos == std::string::npos)
		throw  HttpRequest::HttpRequestException("Header line bad formed", 400);
	std::string key = headerLine.substr(0, colonPos);
	std::string value = headerLine.substr(colonPos + 2);
	this->headers[key] = value;
}
void	HttpRequest::parseHeaders(const std::string &header, size_t pos)
{
	if(pos > header.length())
		throw	HttpRequest::HttpRequestException("Bad formed headers", 404);
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
