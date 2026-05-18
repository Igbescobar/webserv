#include "response/PostResponder.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/FileResponder.hpp"
#include "response/ResponseFactory.hpp"
#include "response/ResponseIO.hpp"
#include <fstream>
#include <sys/stat.h>

HttpResponse PostResponder::handle(const ServerConfig &config,
                                   const LocationConfig *location,
                                   const HttpRequest &req) {

  if (!isRequestValidForUpload(req))
    return ErrorResponseBuilder::build(config, req, 400);

  if (PostResponder::isBodyTooLarge(config, location, req))
    return ErrorResponseBuilder::build(config, req, 413);

  const std::string uploadDir =
      PostResponder::resolveUploadDir(config, location);
  if (!PostResponder::ensureDirExists(uploadDir))
    return ErrorResponseBuilder::build(config, req, 500);

  const std::string fileName = baseNameFromUri(req.getUri());
  const std::string targetPath = ResponseIO::joinPath(uploadDir, fileName);

  if (!PostResponder::writeBodyToFile(targetPath, req.getBody()))
    return ErrorResponseBuilder::build(config, 500);

  HttpResponse resp = ResponseFactory::make(req, config, 201);
  resp.setHeader("Content-Type", "text/plain");
  resp.setBody("Created\n");
  return resp;
}

bool PostResponder::isRequestValidForUpload(const HttpRequest &req) {
  if (!FileResponder::isSafeUri(req.getUri()))
    return false;
  if (req.getBody().empty())
    return false;
  return true;
}

bool PostResponder::isBodyTooLarge(const ServerConfig &config,
                                   const LocationConfig *location,
                                   const HttpRequest &req) {
  long maxSize = PostResponder::resolveMaxBodySize(config, location);
  if (maxSize < 0)
    return false;
  return static_cast<long>(req.getBody().size()) > maxSize;
}

long PostResponder::resolveMaxBodySize(const ServerConfig &config,
                                       const LocationConfig *location) {
  long maxSize = config.getClientMaxBodySize();
  if (location != NULL) {
    long locMax = location->getClientMaxBodySize();
    if (locMax != -1)
      maxSize = locMax;
  }
  return maxSize;
}

std::string PostResponder::resolveUploadDir(const ServerConfig &config,
                                            const LocationConfig *location) {
  if (location != NULL && !location->getUploadPath().empty())
    return location->getUploadPath();
  return ResponseIO::joinPath(FileResponder::getDocumentRoot(config, location),
                              "uploads");
}

bool PostResponder::ensureDirExists(const std::string &dirPath) {
  if (dirPath.empty())
    return false;
  if (FileResponder::isDirectory(dirPath))
    return true;
  return (::mkdir(dirPath.c_str(), 0755) == 0);
}

std::string PostResponder::baseNameFromUri(const std::string &uri) {
  size_t q = uri.find('?');
  std::string u = (q == std::string::npos) ? uri : uri.substr(0, q);

  while (!u.empty() && u[u.size() - 1] == '/')
    u.erase(u.size() - 1);

  size_t slash = u.find_last_of('/');
  std::string base = (slash == std::string::npos) ? u : u.substr(slash + 1);
  if (base.empty())
    return "upload.bin";
  return base;
}

bool PostResponder::writeBodyToFile(const std::string &path,
                                    const std::string &body) {
  std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::trunc);
  if (!ofs.is_open())
    return false;
  ofs.write(body.data(), body.size());
  return ofs.good();
}
