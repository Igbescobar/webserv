#include "response/HttpResponse.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"

HttpResponse::HttpResponse(ServerConfig serverConfig, HttpRequest request)
    : serverConfig(serverConfig), request(request) {
  response = RESPONSE;
}

std::string HttpResponse::getResponse() { return response; }

void HttpResponse::erase(int bytes) { response.erase(0, bytes); }
