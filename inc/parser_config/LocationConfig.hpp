#pragma once

#include <string>
#include <vector>

class LocationConfig {
public:
  LocationConfig();
  LocationConfig(const LocationConfig &other);
  LocationConfig &operator=(const LocationConfig &other);
  ~LocationConfig();

  void setPattern(const std::string &pattern);
  void setRoot(const std::string &root);
  void addIndex(const std::string &index);
  void setAutoIndex(bool index);
  void addAllowedMethod(const std::string &allowedMethod);
  void setReturnTarget(const std::string &returnTarget);
  void setUploadPath(const std::string &uploadPath);
  void setClientMaxBodySize(const long &size);
  void addCgiPassExtensions(const std::string &extension);
  void setReturnCode(int code);

  const std::string &getPattern() const;
  const std::string &getRoot() const;
  const std::vector<std::string> &getIndexes() const;
  bool getAutoIndex() const;
  const std::vector<std::string> &getAllowedMethods() const;
  const std::string &getReturnTarget() const;
  const std::string &getUploadPath() const;
  long getClientMaxBodySize() const;
  const std::vector<std::string> &getCgiPassExtensions() const;
  int getReturnCode() const;

private:
  std::string pattern;
  std::string root;
  std::vector<std::string> index;
  bool autoindex;
  std::vector<std::string> allowedMethods;
  int returnCode;
  std::string returnTarget;
  std::string uploadPath;
  long clientMaxBodySize;
  std::vector<std::string> cgiPassExtensions;
};