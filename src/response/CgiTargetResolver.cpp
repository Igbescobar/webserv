#include "response/CgiTargetResolver.hpp"
#include "request/HttpRequest.hpp"
#include "response/ResponseIO.hpp"

CgiTarget CgiTargetResolver::resolve(const ServerConfig &config,
                                     const HttpRequest &req) {
  CgiTarget target;

  const std::string uriPath = stripQuery(req.getUri());
  if (uriPath.empty() || uriPath.find("..") != std::string::npos)
    return target;

  const LocationConfig *location = config.resolveLocation(uriPath);
  if (location == NULL)
    return target;

  if (!isUriUnderLocation(uriPath, location->getPattern()))
    return target;

  if (!isMethodAllowed(location, req.getMethod()))
    return target;

  const std::string extension = extractExtension(uriPath);
  if (extension.empty())
    return target;

  if (!hasAllowedExtension(*location, extension))
    return target;

  const std::string scriptPath = buildScriptPath(config, *location, uriPath);
  if (scriptPath.empty())
    return target;

  target.isCgi = true;
  target.scriptPath = scriptPath;
  target.location = location;
  return target;
}

std::string CgiTargetResolver::stripQuery(const std::string &uri) {
  const size_t queryPos = uri.find('?');
  if (queryPos == std::string::npos)
    return uri;
  return uri.substr(0, queryPos);
}

std::string CgiTargetResolver::extractExtension(const std::string &uriPath) {
  const size_t slashPos = uriPath.find_last_of('/');
  const size_t dotPos = uriPath.find_last_of('.');

  if (dotPos == std::string::npos)
    return "";
  if (slashPos != std::string::npos && dotPos < slashPos)
    return "";

  return uriPath.substr(dotPos);
}

bool CgiTargetResolver::hasAllowedExtension(const LocationConfig &location,
                                            const std::string &extension) {
  const std::vector<std::string> &allowed = location.getCgiPassExtensions();
  for (size_t i = 0; i < allowed.size(); i++) {
    if (allowed[i] == extension)
      return true;
  }
  return false;
}

bool CgiTargetResolver::isMethodAllowed(const LocationConfig *location,
                                        const std::string &method) {
  if (location == NULL || location->getAllowedMethods().empty())
    return true;

  const std::vector<std::string> &allowed = location->getAllowedMethods();
  for (size_t i = 0; i < allowed.size(); i++) {
    if (allowed[i] == method)
      return true;
  }

  return false;
}

bool CgiTargetResolver::isUriUnderLocation(const std::string &uriPath,
                                           const std::string &pattern) {
  if (pattern.empty())
    return true;
  if (uriPath.compare(0, pattern.size(), pattern) != 0)
    return false;

  if (pattern[pattern.size() - 1] == '/')
    return true;

  if (uriPath.size() == pattern.size())
    return true;

  return uriPath[pattern.size()] == '/';
}

std::string CgiTargetResolver::trimLocationPrefix(const std::string &uriPath,
                                                  const std::string &pattern) {
  if (pattern.empty() || pattern == "/")
    return uriPath;

  if (uriPath.compare(0, pattern.size(), pattern) != 0)
    return "";

  if (uriPath.size() <= pattern.size())
    return "";

  std::string relative = uriPath.substr(pattern.size());
  if (relative.empty())
    return "";

  if (relative[0] != '/')
    relative = "/" + relative;

  return relative;
}

std::string CgiTargetResolver::buildScriptPath(const ServerConfig &config,
                                               const LocationConfig &location,
                                               const std::string &uriPath) {
  std::string root = location.getRoot();
  if (root.empty())
    root = config.getRoot();
  if (root.empty())
    return "";

  const std::string relative = trimLocationPrefix(uriPath, location.getPattern());
  if (relative.empty())
    return "";

  return ResponseIO::joinPath(root, relative);
}
