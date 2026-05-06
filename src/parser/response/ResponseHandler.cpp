#include "parser/response/ResponseHandler.hpp"
#include "parser/response/ErrorResponseBuilder.hpp"
#include "parser/response/FileResponder.hpp"

ResponseHandler::ResponseHandler(const ServerConfig &config,
                                 const IRequest &req)
    : config(config), req(&req), errorCode(0) {}

ResponseHandler::ResponseHandler(const ServerConfig &config, int errorCode)
    : config(config), req(NULL), errorCode(errorCode) {}

std::string ResponseHandler::handle() {
  if (errorCode != 0)
    return ErrorResponseBuilder::build(config, errorCode);

  if (req == NULL)
    return ErrorResponseBuilder::build(config, 500);

  const std::string method = req->getMethod();
  if (method == "GET")
    return handleGet();

  return ErrorResponseBuilder::build(config, 405);
}

std::string ResponseHandler::handleGet() {
  return FileResponder::handleGet(config, *req);
}