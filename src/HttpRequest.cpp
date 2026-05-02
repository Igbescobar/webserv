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
	if(method != "GET" && method != "POST" && method != "DELETE" && method != "HEAD")
		throw HttpRequest::HttpRequestException("Undefined method", 405);
	if(version != "HTTP/1.1")
		throw HttpRequest::HttpRequestException("HTML version not valid", 505);
}

const std::string	extractString(const std::string &Data,const std::string &delimeter)
{
	size_t	stringEnd = Data.find(delimeter);
	if(stringEnd == std::string::npos)
		throw HttpRequest::HttpRequestException("Data not correctly extracted", 400);
	return Data.substr(0, stringEnd);
}

void	HttpRequest::parseFirstLineValues(const std::string &requestLine)
{
	size_t firstLineFirstSpace = requestLine.find(" ");

	checkFind(firstLineFirstSpace, "Bad formed request line", 400);
	size_t firstLineSecondSpace = requestLine.find(" ", firstLineFirstSpace + 1);
	checkFind(firstLineSecondSpace, "Bad formed request line", 400);
	this->method = requestLine.substr(0, firstLineFirstSpace);
	this->uri = requestLine.substr(firstLineFirstSpace + 1, firstLineSecondSpace - firstLineFirstSpace - 1);
	this->version = requestLine.substr(firstLineSecondSpace + 1);
	checkRequestLine();
}

void	HttpRequest::parseHeaderLine(const std::string &headerLine)
{
	size_t colonPos = headerLine.find(":");
	if(colonPos == std::string::npos)
		throw  HttpRequest::HttpRequestException("Header line bad formed", 400);
	std::string key = headerLine.substr(0, colonPos);
	size_t valueStart = headerLine.find_first_not_of(" \t", colonPos + 1);
	std::string value = headerLine.substr(valueStart);
	this->headers[key] = value;
}
void	HttpRequest::parseHeaders(const std::string &header, size_t pos)
{
	if(pos > header.length())
		throw	HttpRequest::HttpRequestException("Bad formed headers", 400);
	while(pos < header.length())
	{
		size_t lineEnd = header.find("\r\n", pos);
		if(lineEnd == std::string::npos)
			lineEnd = header.length();
		parseHeaderLine(header.substr(pos, lineEnd - pos));
		pos = lineEnd + 2;
	}
}

void HttpRequest::parseBody(const std::string &rawData)
{
	size_t bodyStart = rawData.find("\r\n\r\n");
	bodyStart += 4;
	/*if (headers.count("Content-Length"))
	{
	}*/
	//size_t length = std::atoi(headers["Content-Length"].c_str());
	this->body = rawData.substr(bodyStart);
	std::cout<<this->body<<"\n";
}

/*void	HttpRequest::validateBody()
{
	if(this->method == "POST")
	{
		
	}
}*/

void	HttpRequest::parseRawData(const std::string &rawData)
{
	std::string	header = extractString(rawData, "\r\n\r\n");
	std::string	requestLine = extractString(header, "\r\n");
	parseFirstLineValues(requestLine);
	size_t	firstLine = header.find("\r\n");
	checkFind(firstLine, "Bad formed header", 400);
	size_t header_start = firstLine + 2;
	parseHeaders(header, header_start);
	parseBody(rawData);
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
		return "";
	return it->second;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const
{
	return this->headers;
}

