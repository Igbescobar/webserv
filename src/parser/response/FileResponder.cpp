#include "parser/response/FileResponder.hpp"
#include "parser/response/ErrorResponseBuilder.hpp"
#include "parser/response/HttpResponse.hpp"
#include "parser/response/ResponseIO.hpp"

#include <sys/stat.h>
#include <vector>

std::string FileResponder::handleGet(const ServerConfig &config,
                                     const IRequest &req) {
  const std::string uri = req.getUri();
  if (!isSafeUri(uri))
    return ErrorResponseBuilder::build(config, 400);

  std::string filePath = ResponseIO::joinPath(config.getRoot(), uri);

  if (isDirectory(filePath)) {
    const std::string indexPath = getIndexPath(config, filePath);
    if (indexPath.empty())
      return ErrorResponseBuilder::build(config, 404);
    filePath = indexPath;
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

bool FileResponder::isRegularFile(const std::string &path) {
  struct stat s;
  if (stat(path.c_str(), &s) == 0)
    return (S_ISREG(s.st_mode));
  return false;
}

std::string FileResponder::getIndexPath(const ServerConfig &config,
                                        const std::string &dirPath) {
  std::string path = dirPath;
  if (!path.empty() && path[path.length() - 1] != '/')
    path += '/';

  const std::vector<std::string> &indexFiles = config.getIndexFiles();
  for (size_t i = 0; i < indexFiles.size(); i++) {
    const std::string candidate = path + indexFiles[i];
    if (isRegularFile(candidate))
      return candidate;
  }

  const std::string fallback = path + "index.html";
  if (isRegularFile(fallback))
    return fallback;

  return "";
}

std::string FileResponder::buildFileResponse(const std::string &filePath) {
  std::string content;
  if (!ResponseIO::readFile(filePath, content)) {
    // Could be 403 or 500; keep 403 for now as you had before.
    HttpResponse response;
    response.setStatusCode(403);
    response.setHeader("Content-Type", "text/plain");
    response.setBody("Forbidden");
    return response.toString();
  }

  HttpResponse response;
  response.setStatusCode(200);
  response.setHeader("Content-Type", ResponseIO::guessContentType(filePath));
  response.setBody(content);
  return response.toString();
}