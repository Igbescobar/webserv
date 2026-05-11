#include "response/AutoIndex.hpp"
#include "response/ResponseIO.hpp"

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <sys/stat.h>
#include <vector>

static std::string normalizeDirUri(const std::string &uri) {
  std::string base = uri.empty() ? "/" : uri;
  if (base[base.length() - 1] != '/')
    base += '/';
  return base;
}

static bool shouldSkipEntryName(const std::string &name) {
  return (name == "." || name == "..");
}

static bool isDirectoryPath(const std::string &path) {
  struct stat st;
  return (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
}

static std::vector<std::string>
readDirectoryEntries(const std::string &dirPath) {
  std::vector<std::string> entries;

  DIR *d = opendir(dirPath.c_str());
  if (!d)
    return entries;

  for (struct dirent *ent = readdir(d); ent != NULL; ent = readdir(d)) {
    const std::string name = ent->d_name;
    if (shouldSkipEntryName(name))
      continue;
    entries.push_back(name);
  }
  closedir(d);

  std::sort(entries.begin(), entries.end());
  return entries;
}

static std::string makeEntryHref(const std::string &baseUri,
                                 const std::string &name, bool isDir) {
  return baseUri + name + (isDir ? "/" : "");
}

static std::string makeEntryDisplay(const std::string &name, bool isDir) {
  return isDir ? (name + "/") : name;
}

std::string AutoIndex::buildHtml(const std::string &uri,
                                 const std::string &dirPath) {
  const std::vector<std::string> entries = readDirectoryEntries(dirPath);
  if (entries.empty()) {
    DIR *d = opendir(dirPath.c_str());
    if (!d)
      return "";
    closedir(d);
  }

  const std::string baseUri = normalizeDirUri(uri);

  std::stringstream html;
  html << "<!DOCTYPE html>\n"
       << "<html><head><meta charset=\"utf-8\">"
       << "<title>Index of " << baseUri << "</title>"
       << "</head><body>"
       << "<h1>Index of " << baseUri << "</h1>"
       << "<hr><pre>";

  if (baseUri != "/")
    html << "<a href=\"../\">../</a>\n";

  for (size_t i = 0; i < entries.size(); ++i) {
    const std::string &name = entries[i];

    const std::string fullPath = ResponseIO::joinPath(dirPath, name);
    const bool isDir = isDirectoryPath(fullPath);

    const std::string href = makeEntryHref(baseUri, name, isDir);
    const std::string display = makeEntryDisplay(name, isDir);

    html << "<a href=\"" << href << "\">" << display << "</a>\n";
  }

  html << "</pre><hr></body></html>\n";
  return html.str();
}