#include "response/FileResponder.hpp"
#include "request/HttpRequest.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/HttpResponse.hpp"
#include "response/ResponseIO.hpp"

#include <sys/stat.h>
#include <vector>

HttpResponse FileResponder::handleGet(const ServerConfig &config,
                                      const LocationConfig *location,
                                      const HttpRequest &req) {
  const std::string uri = req.getUri();

  if (!isSafeUri(uri))
    return ErrorResponseBuilder::build(config, 400);

  std::string filePath =
      ResponseIO::joinPath(getDocumentRoot(config, location), uri);

  if (isDirectory(filePath)) {
    return handleDirectory(config, location, uri, filePath);
  }

  if (!isRegularFile(filePath))
    return ErrorResponseBuilder::build(config, 404);

  return buildFileResponse(filePath);
}

bool FileResponder::isSafeUri(const std::string &uri) {
  return (uri.find("..") == std::string::npos);
}

bool FileResponder::isDirectory(const std::string &path) {
  struct stat s;
  if (stat(path.c_str(), &s) == 0)
    return (S_ISDIR(s.st_mode));
  return false;
}
HttpResponse FileResponder::handleDirectory(const ServerConfig &config,
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
    // TODO: Build HTML Directory listing all the files inside the foler
    return ErrorResponseBuilder::build(config, 501);
  }

  return ErrorResponseBuilder::build(config, 403);
}

std::string FileResponder::getIndexPath(const ServerConfig &config,
                                        const LocationConfig *location,
                                        const std::string &dirPath) {
  std::string path = dirPath;
  if (!path.empty() && path[path.length() - 1] != '/')
    path += '/';

  if (location != NULL) {
    const std::vector<std::string> &locationIndexes = location->getIndexes();
    for (size_t i = 0; i < locationIndexes.size(); i++) {
      const std::string candidate = path + locationIndexes[i];
      if (isRegularFile(candidate))
        return candidate;
    }
  }

  const std::vector<std::string> &serverIndexes = config.getIndexFiles();
  for (size_t i = 0; i < serverIndexes.size(); i++) {
    const std::string candidate = path + serverIndexes[i];
    if (isRegularFile(candidate))
      return candidate;
  }

  const std::string fallback = path + "index.html";
  if (isRegularFile(fallback))
    return fallback;

  return "";
}

std::string FileResponder::getDocumentRoot(const ServerConfig &config,
                                           const LocationConfig *location) {
  if (location != NULL && !location->getRoot().empty())
    return location->getRoot();
  return config.getRoot();
}

bool FileResponder::isRegularFile(const std::string &path) {
  struct stat s;
  if (stat(path.c_str(), &s) == 0)
    return (S_ISREG(s.st_mode));
  return false;
}

HttpResponse FileResponder::buildFileResponse(const std::string &filePath) {
  std::string content;
  if (!ResponseIO::readFile(filePath, content)) {
    HttpResponse response;
    response.setStatusCode(403);
    response.setHeader("Content-Type", "text/plain");
    response.setBody("Forbidden");
    return response;
  }

  HttpResponse response;
  response.setStatusCode(200);
  response.setHeader("Content-Type", ResponseIO::guessContentType(filePath));
  response.setBody(content);
  return response;
}