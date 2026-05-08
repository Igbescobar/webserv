#pragma once

#include <string>

class ResponseIO {
public:
  static std::string joinPath(const std::string &base, const std::string &rel);
  static bool readFile(const std::string &path, std::string &out);
  static std::string guessContentType(const std::string &path);
};