#include "response/ResponseHandler.hpp"
#include "request/HttpRequest.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/FileResponder.hpp"
#include "response/HttpResponse.hpp"

ResponseHandler::ResponseHandler(const ServerConfig &config,
                                 const HttpRequest &req)
    : config(config), req(&req), errorCode(0) {}

ResponseHandler::ResponseHandler(const ServerConfig &config, int errorCode)
    : config(config), req(NULL), errorCode(errorCode) {}

HttpResponse ResponseHandler::handle() {
  if (errorCode != 0)
    return ErrorResponseBuilder::build(config, errorCode);

  if (req == NULL)
    return ErrorResponseBuilder::build(config, 500);

  const LocationConfig *location = config.resolveLocation(req->getUri());

  const std::string method = req->getMethod();
  if (!isMethodAllowed(location, method))
    return ErrorResponseBuilder::build(config, 405);

  HttpResponse redirection;
  if (handleRedirect(location, redirection))
    return redirection;

  if (method == "GET")
    return FileResponder::handleGet(config, location, *req);

  return ErrorResponseBuilder::build(config, 405);
}

bool ResponseHandler::isMethodAllowed(const LocationConfig *location,
                                      const std::string &method) const {
  if (location == NULL || location->getAllowedMethods().empty())
    return true;

  const std::vector<std::string> &allowed = location->getAllowedMethods();
  for (size_t i = 0; i < allowed.size(); i++) {
    if (allowed[i] == method)
      return true;
  }
  return false;
}

bool ResponseHandler::handleRedirect(const LocationConfig *location,
                                     HttpResponse &response) const {
  if (location != NULL && !location->getReturnTarget().empty()) {
    HttpResponse response;
    response.setStatusCode(301);
    response.setHeader("Location", location->getReturnTarget());
    return true;
  }
  return false;
}