#pragma once

#include "parser/config/ServerConfig.hpp"
#include <cstdlib>
#include <string>

#define DELIMETER "\r\n\r\n"

typedef enum e_state { INCOMPLETE, COMPLETE, ERROR } t_state;

class HttpRequest {
private:
  ServerConfig serverConfig;
  std::string buf;
  t_state state;
  int errorCode;
  std::string method;
  std::string uri;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string body;
  size_t headerStart;

public:
  HttpRequest();
  HttpRequest(ServerConfig serverConfig);
  ~HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);

  void append(std::string chunk);

  void  parse(std::string chunck);
  void  parseRequestLine();
  void	parseRequestLineValues(const std::string &header);
  void	parseHeaders();
  void	parseHeaderLine(const std::string &headerLine);
  void	parseBody();
  void	parseChunkedBody(size_t pos);
  void	checkRequestLine();
  void	checkRequestHeaders();
  void  checkIfLineComplete(std::string chunck);
  void	printState();
  bool	isBodyComplete();

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
		std::string msg() const { return _msg;}
  };
  std::string getMethod() const;
  std::string getUri() const;
  std::string getVersion() const;
  std::string getHeader(const std::string &key) const;
  std::map<std::string, std::string> getHeaders() const;
  std::string getBody() const;

  t_state getState();

  int getErrorCode();

  ServerConfig getServerConfig();

  std::string getRequest();
};
