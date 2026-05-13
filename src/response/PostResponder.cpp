#include "response/PostResponder.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/FileResponder.hpp"
#include "response/ResponseIO.hpp"
#include <fstream>
#include <sys/stat.h>

HttpResponse PostResponder::handle(const ServerConfig &config,
                                   const LocationConfig *location,
                                   const HttpRequest &req) {

  if (!FileResponder::isSafeUri(req.getUri()))
    return ErrorResponseBuilder::build(config, 400);

  const std::string body = req.getBody();
  if (body.empty())
    return ErrorResponseBuilder::build(config, 400);

  std::string uploadDir;
  if (location != NULL && !location->getUploadPath().empty())
    uploadDir = location->getUploadPath();
  else
    uploadDir = ResponseIO::joinPath(
        FileResponder::getDocumentRoot(config, location), "uploads");

  if (!ensureDirExists(uploadDir))
    return ErrorResponseBuilder::build(config, 500);

  const std::string fileName = baseNameFromUri(req.getUri());
  const std::string targetPath = ResponseIO::joinPath(uploadDir, fileName);

  std::ofstream ofs(targetPath.c_str(), std::ios::binary | std::ios::trunc);
  if (!ofs.is_open())
    return ErrorResponseBuilder::build(config, 500);

  ofs.write(body.data(), body.size());
  if (!ofs.good())
    return ErrorResponseBuilder::build(config, 500);

  HttpResponse resp;
  resp.setStatusCode(201);
  resp.setHeader("Content-Type", "text/plain");
  resp.setBody("Created\n");
  return resp;
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