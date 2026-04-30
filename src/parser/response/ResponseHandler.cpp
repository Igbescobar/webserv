#include "parser/response/ResponseHandler.hpp"

#include "parser/request/IRequest.hpp"
#include "parser/response/HttpResponse.hpp"

ResponseHandler::ResponseHandler(const std::string &docRoot)
    : docRoot(docRoot) {}

std::string ResponseHandler::handle(const IRequest &req) {
  const std::string method = req.getMethod();

  if (method == "GET")
    return handleGet(req);

  return buildError(405, "Method not allowed");
}

std::string ResponseHandler::handleGet(const IRequest &req) {}