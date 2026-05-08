#include "response/ErrorResponseBuilder.hpp"
#include "response/HttpResponse.hpp"
#include "response/ResponseIO.hpp"

#include <map>
#include <sstream>

std::string ErrorResponseBuilder::build(const ServerConfig &config,
                                        int statusCode) {
  return build(config, statusCode, reasonPhrase(statusCode));
}

std::string ErrorResponseBuilder::build(const ServerConfig &config,
                                        int statusCode,
                                        const std::string &message) {
  std::string body;
  std::string contentType;

  if (tryBuildConfiguredErrorPage(config, statusCode, body, contentType)) {
    HttpResponse response;
    response.setStatusCode(statusCode);
    response.setHeader("Content-Type", contentType);
    response.setBody(body);
    return response.toString();
  }

  HttpResponse response;
  response.setStatusCode(statusCode);
  response.setHeader("Content-Type", "text/html");
  response.setBody(defaultErrorHtml(statusCode, message));
  return response.toString();
}

const char *ErrorResponseBuilder::reasonPhrase(int statusCode) {
  switch (statusCode) {
  case 400:
    return "Bad Request";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 413:
    return "Payload Too Large";
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  default:
    return "Error";
  }
}

bool ErrorResponseBuilder::tryBuildConfiguredErrorPage(
    const ServerConfig &config, int statusCode, std::string &outBody,
    std::string &outContentType) {
  const std::map<int, std::string> &pages = config.getErrorPages();
  std::map<int, std::string>::const_iterator it = pages.find(statusCode);
  if (it == pages.end())
    return false;

  const std::string &configured = it->second;
  if (configured.empty())
    return false;

  const std::string errorPath =
      ResponseIO::joinPath(config.getRoot(), configured);

  std::string content;
  if (!ResponseIO::readFile(errorPath, content))
    return false;

  outBody = content;
  outContentType = ResponseIO::guessContentType(errorPath);
  return true;
}

std::string ErrorResponseBuilder::defaultErrorHtml(int statusCode,
                                                   const std::string &message) {
  std::stringstream body;
  body << "<html><head><title>" << statusCode << " " << message
       << "</title></head>"
       << "<body><center><h1>" << statusCode << " " << message
       << "</h1></center>"
       << "<hr><center>webserv</center></body></html>";
  return body.str();
}