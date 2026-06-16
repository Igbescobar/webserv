#include "response/PostResponder.hpp"
#include "response/ErrorResponseBuilder.hpp"
#include "response/FileResponder.hpp"
#include "response/ResponseFactory.hpp"
#include "response/ResponseIO.hpp"
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>
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

  std::string fileName = baseNameFromUri(req.getUri());
  std::string fileData = req.getBody();

  if (isMultipart(req)) {
    if (!parseMultipartFile(req, fileName, fileData))
      return ErrorResponseBuilder::build(config, req, 400);
  } else {
    fileName = baseNameFromUri(req.getUri());
    fileData = req.getBody();
  }

  const std::string targetPath = ResponseIO::joinPath(uploadDir, fileName);

  if (!PostResponder::writeBodyToFile(targetPath, fileData))
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
  if (dirPath.empty()) {
    std::cerr << "PostResponder::ensureDirExists: empty dirPath" << std::endl;
    return false;
  }
  if (FileResponder::isDirectory(dirPath))
    return true;
  if (::mkdir(dirPath.c_str(), 0755) == 0)
    return true;
  int err = errno;
  std::cerr << "PostResponder::ensureDirExists: mkdir(\"" << dirPath
            << "\") failed: " << strerror(err) << " (" << err << ")"
            << std::endl;
  return false;
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
  if (!ofs.is_open()) {
    int err = errno;
    std::cerr << "PostResponder::writeBodyToFile: open(\"" << path
              << "\") failed: " << strerror(err) << " (" << err << ")"
              << std::endl;
    return false;
  }
  ofs.write(body.data(), body.size());
  if (!ofs.good()) {
    int err = errno;
    std::cerr << "PostResponder::writeBodyToFile: write(\"" << path
              << "\") failed: " << strerror(err) << " (" << err << ")"
              << std::endl;
    return false;
  }
  return true;
}

std::string PostResponder::toLower(const std::string &s) {
  std::string out = s;
  for (size_t i = 0; i < out.size(); ++i) {
    char c = out[i];
    if (c >= 'A' && c <= 'Z')
      out[i] = static_cast<char>(c - 'A' + 'a');
  }
  return out;
}

std::string PostResponder::sanitizeFileName(const std::string &name) {
  std::string out = name;

  size_t slash = out.find_last_of('/');
  if (slash != std::string::npos)
    out = out.substr(slash + 1);

  size_t bslash = out.find_last_of('\\');
  if (bslash != std::string::npos)
    out = out.substr(bslash + 1);

  if (out.empty())
    return "upload.bin";
  return out;
}

bool PostResponder::isMultipart(const HttpRequest &req) {
  return toLower(req.getHeader("content-type")).find("multipart/form-data") !=
         std::string::npos;
}

bool PostResponder::parseMultipartFile(const HttpRequest &req,
                                       std::string &outFileName,
                                       std::string &outFileData) {
  const std::string &body = req.getBody();

  size_t start = body.find("filename=\"");
  if (start == std::string::npos)
    return false;

  start += 10;

  size_t end = body.find('"', start);
  if (end == std::string::npos)
    return false;

  outFileName = body.substr(start, end - start);
  outFileName = sanitizeFileName(outFileName);

  size_t dataStart = body.find("\r\n\r\n", end);
  if (dataStart == std::string::npos)
    return false;

  dataStart += 4;

  size_t dataEnd = body.rfind("\r\n------");
  if (dataEnd == std::string::npos)
    return false;

  outFileData = body.substr(dataStart, dataEnd - dataStart);

  return true;
}
