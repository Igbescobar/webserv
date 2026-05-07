#include "response/HttpResponse.hpp"
#include "parser/config/ServerConfig.hpp"
#include "request/HttpRequest.hpp"

HttpResponse::HttpResponse(ServerConfig serverConfig, HttpRequest request)
    : serverConfig(serverConfig), request(request) {}

std::string HttpResponse::getResponse() { return RESPONSE; }
