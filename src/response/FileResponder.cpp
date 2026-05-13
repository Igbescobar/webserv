#include "response/FileResponder.hpp"
#include "request/HttpRequest.hpp"
#include "response/DeleteResponder.hpp"
#include "response/GetResponder.hpp"
#include "response/HttpResponse.hpp"
#include "response/PostResponder.hpp"
#include <sys/stat.h>

HttpResponse FileResponder::handleGet(const ServerConfig &config,
                                      const LocationConfig *location,
                                      const HttpRequest &req) {
  return GetResponder::handle(config, location, req);
}

HttpResponse FileResponder::handlePost(const ServerConfig &config,
                                       const LocationConfig *location,
                                       const HttpRequest &req) {
  return PostResponder::handle(config, location, req);
}

HttpResponse FileResponder::handleDelete(const ServerConfig &config,
                                         const LocationConfig *location,
                                         const HttpRequest &req) {
  return DeleteResponder::handle(config, location, req);
}

bool FileResponder::isSafeUri(const std::string &uri) {
  return (uri.find("..") == std::string::npos);
}

std::string FileResponder::getDocumentRoot(const ServerConfig &config,
                                           const LocationConfig *location) {
  if (location != NULL && !location->getRoot().empty())
    return location->getRoot();
  return config.getRoot();
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