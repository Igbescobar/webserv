#include "response/HttpResponse.hpp"
#include "parser/config/ServerConfig.hpp"

HttpResponse::HttpResponse(ServerConfig &serverConfig,
                           const std::string &request)
    : serverConfig(serverConfig), request(request) {}
