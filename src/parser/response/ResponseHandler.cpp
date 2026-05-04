#include "parser/response/ResponseHandler.hpp"
#include "parser/request/IRequest.hpp"
#include "parser/response/HttpResponse.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

ResponseHandler::ResponseHandler(const ServerConfig &config) : config(config) {}

std::string ResponseHandler::handle(const IRequest &req) {
  const std::string method = req.getMethod();
  if (method == "GET")
    return (handleGet(req));
  return (buildError(405, "Method not allowed"));
}

std::string ResponseHandler::handleGet(const IRequest &req) {
  const std::string uri = req.getUri();
  if (!isSafeUri(uri))
    return (buildError(400, "Bad Request"));
  std::string filePath = joinPath(config.getRoot(), uri);
  if (isDirectory(filePath))
    filePath = getIndexPath(filePath);
  if (!isRegularFile(filePath))
    return (buildError(404, "Not Found"));
  return (buildFileResponse(filePath));
}

bool ResponseHandler::isSafeUri(const std::string &uri) const {
  return (uri.find("..") == std::string::npos);
}

bool ResponseHandler::isDirectory(const std::string &path) const {
  struct stat s;
  if (stat(path.c_str(), &s) == 0)
    return (S_ISDIR(s.st_mode));
  return (false);
}

bool ResponseHandler::isRegularFile(const std::string &path) const {
  struct stat s;
  if (stat(path.c_str(), &s) == 0) {
    return (S_ISREG(s.st_mode));
  }
  return (false);
}

std::string ResponseHandler::getIndexPath(const std::string &dirPath) const {
  std::string path = dirPath;
  if (!path.empty() && path[path.length() - 1] != '/')
    path += '/';

  const std::vector<std::string> &indexFiles = config.getIndexFiles();

  return (path + "index.html");
}

std::string ResponseHandler::buildFileResponse(const std::string &filePath) {
  HttpResponse response;

  std::string content;
  if (!readFile(filePath, content))
    return (buildError(403, "Forbidden"));
  response.setStatusCode(200);
  response.setHeader("Content-Type", guessContentType(filePath));
  response.setBody(content);
  return (response.toString());
}

std::string ResponseHandler::buildError(int statusCode,
                                        const std::string &message) {
  HttpResponse response;

  response.setStatusCode(statusCode);
  response.setHeader("Content-Type", "text/html");
  std::stringstream body;
  body << "<html><head><title>" << statusCode << " " << message
       << "</title></head>"
       << "<body><center><h1>" << statusCode << " " << message
       << "</h1></center>"
       << "<hr><center>webserv</center></body></html>";
  response.setBody(body.str());
  return (response.toString());
}

std::string ResponseHandler::joinPath(const std::string &base,
                                      const std::string &rel) {
  bool baseHasSlash;
  bool relHasSlash;

  if (base.empty())
    return (rel);
  if (rel.empty())
    return (base);
  baseHasSlash = (base[base.length() - 1] == '/');
  relHasSlash = (rel[0] == '/');
  if (baseHasSlash && relHasSlash)
    return (base + rel.substr(1));
  else if (!baseHasSlash && !relHasSlash)
    return (base + "/" + rel);
  return (base + rel);
}

bool ResponseHandler::readFile(const std::string &path, std::string &out) {
  std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
  if (!file)
    return (false);
  std::ostringstream ss;
  ss << file.rdbuf();
  out = ss.str();
  return (true);
}

std::string ResponseHandler::guessContentType(const std::string &path) {
  size_t dotPos = path.find_last_of('.');
  if (dotPos == std::string::npos)
    return ("application/octet-stream");

  std::string ext = path.substr(dotPos + 1);
  for (size_t i = 0; i < ext.length(); ++i)
    ext[i] = std::tolower(ext[i]);

  if (ext == "html" || ext == "htm")
    return ("text/html");
  if (ext == "css")
    return ("text/css");
  if (ext == "js")
    return ("application/javascript");
  if (ext == "jpg" || ext == "jpeg")
    return ("image/jpeg");
  if (ext == "png")
    return ("image/png");
  if (ext == "txt")
    return ("text/plain");

  return ("application/octet-stream");
}