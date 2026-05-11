#pragma once

#include <string>

class AutoIndex {
public:
  static std::string buildHtml(const std::string &uri,
                               const std::string &dirPath);

private:
  //   static std::string escapeHtml(const std::string &in);
};