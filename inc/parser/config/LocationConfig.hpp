#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
public:
  enum MatchType { PREFIX, EXACT, REGEX };

  LocationConfig();
  LocationConfig(const LocationConfig &other);
  LocationConfig &operator=(const LocationConfig &other);
  ~LocationConfig();

  void setPattern(const std::string &pattern);
  void setMatchType(MatchType type);
  void setRoot(const std::string &root);
  void addIndex(const std::string &index);
  void setAutoIndex(bool index);
  void addAllowedMethod(const std::string &allowedMethod);
  void setReturnTarget(const std::string &returnTarget);
  void setUploadPath(const std::string &uploadPath);
  void setClientMaxBodySize(const long &size);
  void addCgiPassExtensions(const std::string &extension);

  const std::string &getPattern() const;
  MatchType getMatchType() const;
  const std::string &getRoot() const;
  const std::vector<std::string> &getIndexes() const;
  bool getAutoIndex() const;
  const std::vector<std::string> &getAllowedMethods() const;
  const std::string &getReturnTarget() const;
  const std::string &getUploadPath() const;
  long getClientMaxBodySize() const;
  const std::vector<std::string> &getCgiPassExtensions() const;

private:
  std::string pattern;
  MatchType matchType;
  std::string root;
  std::vector<std::string> index;
  bool autoindex;
  std::vector<std::string> allowedMethods;
  std::string returnTarget;
  std::string uploadPath;
  long clientMaxBodySize;
  std::vector<std::string> cgiPassExtensions;
};

#endif