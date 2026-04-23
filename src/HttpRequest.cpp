#include <iostream>
#include "HttpRequest.hpp"

//HttpRequest::HttpRequest(): method(method), uri(uri), version(version), headers(headers), body(body)
//{}

HttpRequest::HttpRequest(const std::string &rawData)
{
	std::cout<<"HtttpRequest constructor called"<<std::endl;
	parseRawData(rawData);
}

HttpRequest::~HttpRequest()
{
	std::cout<<"Http Destructor called"<<std::endl;
}

void	HttpRequest::parseRawData(const std::string &rawData)
{
	size_t	headerEnd = rawData.find("\r\n\r\n");
	if(headerEnd == std::string::npos)
		std::cout<<"Error, headerEnd not found"<<std::endl;

	std::string header = rawData.substr(0, headerEnd);

	size_t	requestEnd = header.find("\r\n");
	if(requestEnd == std::string::npos)
		std::cout<<"Error, requestEnd not found"<<std::endl;
	
	std::string requestLine = header.substr(0, requestEnd);

	size_t firstLineFirstSpace = header.find(" ");
	size_t firstLineSecondSpace = header.find(" ", firstLineFirstSpace + 1);

	this->method = requestLine.substr(0, firstLineFirstSpace);
	this->uri = requestLine.substr(firstLineFirstSpace, firstLineSecondSpace - firstLineFirstSpace);
	std::cout<<this->method<<std::endl;
	std::cout<<this->uri<<std::endl;
}
