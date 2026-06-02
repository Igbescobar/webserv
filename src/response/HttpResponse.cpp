#include "response/HttpResponse.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(ServerConfig serverConfig, HttpRequest request)
    : serverConfig(serverConfig), request(request) {
  response = RESPONSE;
}

HttpResponse::HttpResponse(ServerConfig serverConfig, int errorCode)
    : serverConfig(serverConfig), errorCode(errorCode) {
  response = RESPONSE;
}

HttpResponse::HttpResponse(const HttpResponse &other) {
  serverConfig = other.serverConfig;
  request = other.request;
  response = other.response;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &other) {
  serverConfig = other.serverConfig;
  request = other.request;
  response = other.response;
  return *this;
}

std::string HttpResponse::getResponse() { return response; }
int HttpResponse::getErrorCode() { return errorCode; }

void HttpResponse::erase(int bytes) { response.erase(0, bytes); }

bool HttpResponse::empty() { return response.empty(); }
