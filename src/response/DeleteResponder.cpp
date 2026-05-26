#include "response/DeleteResponder.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/FileResponder.hpp"
#include "response/ResponseFactory.hpp"
#include "response/ResponseIO.hpp"
#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>

HttpResponse DeleteResponder::handle(const ServerConfig &config,
                                     const LocationConfig *location,
                                     const HttpRequest &req) {
  const std::string uri = req.getUri();

  if (!FileResponder::isSafeUri(uri))
    return ErrorResponseBuilder::build(config, req, 400);

  const std::string docRoot = FileResponder::getDocumentRoot(config, location);
  const std::string fsPath = ResponseIO::joinPath(docRoot, uri);

  struct stat st;
  if (!statPath(fsPath, st))
    return ErrorResponseBuilder::build(config, req, 404);

  if (FileResponder::isDirectory(fsPath))
    return ErrorResponseBuilder::build(config, req, 403);

  if (::unlink(fsPath.c_str()) != 0) {
    if (errno == ENOENT)
      return ErrorResponseBuilder::build(config, req, 404);
    if (errno == EACCES || errno == EPERM)
      return ErrorResponseBuilder::build(config, req, 403);
    return ErrorResponseBuilder::build(config, req, 500);
  }

  HttpResponse resp = ResponseFactory::make(req, config, 204);
  resp.setHeader("Content-Length", "0");
  resp.clearBody();
  return resp;
}

bool DeleteResponder::statPath(const std::string &path, struct stat &st) {
  return ::stat(path.c_str(), &st) == 0;
}