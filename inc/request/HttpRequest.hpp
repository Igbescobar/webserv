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

  void  parseRequestLine();
  void	parseRequestLineValues(const std::string &header);
  void	parseHeaders();
  bool	isValidHeaderKey(const std::string &key);
  std::string	toLowerCase(const std::string &str);
  std::string	trim(const std::string &str);
  void	parseHeaderLine(const std::string &headerLine);
  void	parseBody();
  void	parseChunkedBody(size_t pos);
  void	checkRequestLine();
  void	checkRequestHeaders();
  void  checkIfLineComplete(std::string chunck);
  void	printState();
  bool	isBodyComplete();

public:
  HttpRequest();
  HttpRequest(ServerConfig serverConfig);
  ~HttpRequest();
  HttpRequest(const HttpRequest &other);
  HttpRequest &operator=(const HttpRequest &other);

  void append(const std::string &chunk);
  const std::string &getMethod() const;
  const std::string &getUri() const;
  const std::string &getVersion() const;
  const std::string &getHeader(const std::string &key) const;
  const std::map<std::string, std::string>&getHeaders() const;
  const ServerConfig &getServerConfig() const;
  const std::string &getBody() const;
  t_state getState() const;
  int getErrorCode() const;
  const std::string &getRequest() const;
};
