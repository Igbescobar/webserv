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

  if (method == "GET" || method == "HEAD") {
    HttpResponse resp = FileResponder::handleGet(config, location, *req);
    if (method == "HEAD")
      resp.clearBody();
    return resp;
  }

  if (method == "POST")
    return FileResponder::handlePost(config, location, *req);

  if (method == "DELETE")
    return FileResponder::handleDelete(config, location, *req);

  return ErrorResponseBuilder::build(config, *req, 404);
}

bool ResponseHandler::isMethodAllowed(const LocationConfig *location,
                                      const std::string &method) const {
  if (location == NULL || location->getAllowedMethods().empty())
    return true;

  bool getMethod = false;

  const std::vector<std::string> &allowed = location->getAllowedMethods();

  for (size_t i = 0; i < allowed.size(); i++) {
    if (allowed[i] == method)
      return true;
    if (allowed[i] == "GET")
      getMethod = true;
  }
  if (method == "HEAD" && getMethod)
    return true;
  return false;
}

bool ResponseHandler::handleRedirect(const LocationConfig *location,
                                     HttpResponse &response) const {
  if (location != NULL && !location->getReturnTarget().empty()) {
    int code = location->getReturnCode();
    if (code == 0)
      code = 301;

    response.setStatusCode(code);
    response.setHeader("Location", location->getReturnTarget());
    return true;
  }
  return false;
}