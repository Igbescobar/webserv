#include "response/ResponseFactory.hpp"
#include <ctime>

HttpResponse ResponseFactory::make(const HttpRequest &req,
                                   const ServerConfig &config, int statusCode) {
  (void)config;

  HttpResponse resp;
  resp.setStatusCode(statusCode);

  resp.setHeader("Date", httpDate());
  resp.setHeader("Server", "webserv");
  resp.setHeader("Connection", connectionValue(req));
  resp.setHeader("X-Content-Type-Options", "nosniff");

  return resp;
}

HttpResponse ResponseFactory::makeWithoutRequest(const ServerConfig &config,
                                                 int statusCode) {
  (void)config;

  HttpResponse resp;
  resp.setStatusCode(statusCode);

  resp.setHeader("Date", httpDate());
  resp.setHeader("Server", "webserv");
  resp.setHeader("Connection", "close");
  resp.setHeader("X-Content-Type-Options", "nosniff");

  return resp;
}

std::string ResponseFactory::httpDate() {
  std::time_t now = std::time(NULL);

  std::tm *ptm = std::gmtime(&now);

  char buf[128];
  std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", ptm);

  return std::string(buf);
}

std::string ResponseFactory::connectionValue(const HttpRequest &req) {
  const std::string c = req.getHeader("connection");
  if (c == "keep-alive" || c == "Keep-Alive")
    return "keep-alive";
  return "close";
}