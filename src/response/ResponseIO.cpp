#include "response/ResponseIO.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <sstream>

std::string ResponseIO::joinPath(const std::string &base,
                                 const std::string &rel) {
  if (base.empty())
    return rel;
  if (rel.empty())
    return base;

  const bool baseHasSlash = (base[base.length() - 1] == '/');
  const bool relHasSlash = (rel[0] == '/');

  if (baseHasSlash && relHasSlash)
    return base + rel.substr(1);
  if (!baseHasSlash && !relHasSlash)
    return base + "/" + rel;
  return base + rel;
}

bool ResponseIO::readFile(const std::string &path, std::string &out) {
  std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
  if (!file)
    return false;
  std::ostringstream ss;
  ss << file.rdbuf();
  out = ss.str();
  return true;
}

std::string ResponseIO::guessContentType(const std::string &path) {
  size_t dotPos = path.find_last_of('.');
  if (dotPos == std::string::npos)
    return "application/octet-stream";

  std::string ext = path.substr(dotPos + 1);
  for (size_t i = 0; i < ext.length(); ++i)
    ext[i] =
        static_cast<char>(std::tolower(static_cast<unsigned char>(ext[i])));

  if (ext == "html" || ext == "htm")
    return "text/html";
  if (ext == "css")
    return "text/css";
  if (ext == "js")
    return "application/javascript";
  if (ext == "jpg" || ext == "jpeg")
    return "image/jpeg";
  if (ext == "png")
    return "image/png";
  if (ext == "txt")
    return "text/plain";
  return "application/octet-stream";
}