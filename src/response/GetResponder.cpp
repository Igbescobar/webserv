#include "response/GetResponder.hpp"
#include "response/AutoIndex.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/FileResponder.hpp"
#include "response/ResponseIO.hpp"
#include <sys/stat.h>
#include <vector>

HttpResponse GetResponder::handle(const ServerConfig &config,
                                  const LocationConfig *location,
                                  const HttpRequest &req) {
  const std::string uri = req.getUri();

  if (!FileResponder::isSafeUri(uri))
    return ErrorResponseBuilder::build(config, 400);

  std::string filePath = ResponseIO::joinPath(
      FileResponder::getDocumentRoot(config, location), uri);

  if (FileResponder::isDirectory(filePath))
    return handleDirectory(config, location, uri, filePath);

  if (!FileResponder::isRegularFile(filePath))
    return ErrorResponseBuilder::build(config, 404);

  return buildFileResponse(filePath);
}

HttpResponse GetResponder::handleDirectory(const ServerConfig &config,
                                           const LocationConfig *location,
                                           const std::string &uri,
                                           const std::string &dirPath) {
  const std::string indexPath = getIndexPath(config, location, dirPath);
  if (!indexPath.empty())
    return buildFileResponse(indexPath);

  if (uri.empty() || uri[uri.length() - 1] != '/') {
    HttpResponse response;
    response.setStatusCode(301);
    response.setHeader("Location", uri + "/");
    return response;
  }

  if (location != NULL && location->getAutoIndex()) {
    const std::string html = AutoIndex::buildHtml(uri, dirPath);
    if (html.empty())
      return ErrorResponseBuilder::build(config, 403);

    HttpResponse response;
    response.setStatusCode(200);
    response.setHeader("Content-Type", "text/html");
    response.setBody(html);
    return response;
  }

  return ErrorResponseBuilder::build(config, 403);
}

std::string GetResponder::getIndexPath(const ServerConfig &config,
                                       const LocationConfig *location,
                                       const std::string &dirPath) {
  std::string path = dirPath;
  if (!path.empty() && path[path.length() - 1] != '/')
    path += '/';

  if (location != NULL) {
    const std::vector<std::string> &locationIndexes = location->getIndexes();
    for (size_t i = 0; i < locationIndexes.size(); i++) {
      const std::string candidate = path + locationIndexes[i];
      if (FileResponder::isRegularFile(candidate))
        return candidate;
    }
  }

  const std::vector<std::string> &serverIndexes = config.getIndexFiles();
  for (size_t i = 0; i < serverIndexes.size(); i++) {
    const std::string candidate = path + serverIndexes[i];
    if (FileResponder::isRegularFile(candidate))
      return candidate;
  }

  const std::string fallback = path + "index.html";
  if (FileResponder::isRegularFile(fallback))
    return fallback;

  return "";
}

HttpResponse GetResponder::buildFileResponse(const std::string &filePath) {
  std::string content;
  if (!ResponseIO::readFile(filePath, content)) {
    HttpResponse response;
    response.setStatusCode(403);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Server", "webserv/1.0");
    response.setHeader("Date", ResponseIO::getCurrentDate());
    response.setBody("Forbidden");
    return response;
  }

  HttpResponse response;
  response.setStatusCode(200);
  response.setHeader("Content-Type", ResponseIO::guessContentType(filePath));
  response.setHeader("Server", "webserv/1.0");
  response.setHeader("Date", ResponseIO::getCurrentDate());
  response.setBody(content);
  return response;
}