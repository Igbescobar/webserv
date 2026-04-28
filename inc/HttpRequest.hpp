#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "IRequest.hpp"
#include <cctype>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <cstdlib>

class ParseError : public std::runtime_error {
public:
  ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

class HttpRequest : public IRequest {
private:
  std::string method;
  std::string uri;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string body;

  void parseRawData(const std::string &rawData);

public:
  HttpRequest(const std::string &rawData);
  ~HttpRequest();
  void	parseFirstLineValues(const std::string &header);
  void	parseHeaders(const std::string &header, size_t pos);
  void	parseHeaderLine(const std::string &headerLine);
  void	parseBody(const std::string &rawData);
  void	checkRequestLine();
  std::string getMethod() const;
  std::string getUri() const;
  std::string getVersion() const;
  std::string getHeader(const std::string &key) const;
  std::map<std::string, std::string> getHeaders() const;
  std::string getBody() const;
  class HttpRequestException: public std::exception
  {
	private:
		std::string _msg;
		int		_code;
	public:
		HttpRequestException(std::string msg, int code): _msg(msg), _code(code){}
		~HttpRequestException() throw() {} 
		const	char* what() const throw(){ return _msg.c_str();}
		int code() const { return _code;}
  };
};

#endif
