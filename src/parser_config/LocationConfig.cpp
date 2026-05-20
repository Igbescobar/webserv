#include "parser_config/LocationConfig.hpp"
#include <string>

LocationConfig::LocationConfig()
    : autoindex(false), returnCode(0), clientMaxBodySize(-1) {}

LocationConfig::LocationConfig(const LocationConfig &other)
    : pattern(other.pattern), root(other.root), index(other.index),
      autoindex(other.autoindex), allowedMethods(other.allowedMethods),
      returnCode(other.returnCode), returnTarget(other.returnTarget),
      uploadPath(other.uploadPath), cgiPassExtensions(other.cgiPassExtensions) {
}

LocationConfig &LocationConfig::operator=(const LocationConfig &other) {
  if (this != &other) {
    pattern = other.pattern;
    root = other.root;
    index = other.index;
    autoindex = other.autoindex;
    allowedMethods = other.allowedMethods;
    returnCode = other.returnCode;
    returnTarget = other.returnTarget;
    uploadPath = other.uploadPath;
    clientMaxBodySize = other.clientMaxBodySize;
    cgiPassExtensions = other.cgiPassExtensions;
  }
  return *this;
}

LocationConfig::~LocationConfig() {}

void LocationConfig::setPattern(const std::string &patternValue) {
  pattern = patternValue;
}

void LocationConfig::setRoot(const std::string &rootValue) { root = rootValue; }

void LocationConfig::addIndex(const std::string &indexValue) {
  index.push_back(indexValue);
}

void LocationConfig::setAutoIndex(bool enabled) { autoindex = enabled; }

void LocationConfig::addAllowedMethod(const std::string &allowedMethod) {
  allowedMethods.push_back(allowedMethod);
}

void LocationConfig::setReturnCode(int code) { this->returnCode = code; }

void LocationConfig::setReturnTarget(const std::string &returnTargetValue) {
  returnTarget = returnTargetValue;
}

void LocationConfig::setUploadPath(const std::string &uploadPathValue) {
  uploadPath = uploadPathValue;
}

void LocationConfig::setClientMaxBodySize(const long &size) {
  this->clientMaxBodySize = size;
}

void LocationConfig::addCgiPassExtensions(const std::string &extension) {
  this->cgiPassExtensions.push_back(extension);
}

const std::string &LocationConfig::getPattern() const { return pattern; }

const std::string &LocationConfig::getRoot() const { return root; }

const std::vector<std::string> &LocationConfig::getIndexes() const {
  return index;
}

bool LocationConfig::getAutoIndex() const { return autoindex; }

const std::vector<std::string> &LocationConfig::getAllowedMethods() const {
  return allowedMethods;
}

int LocationConfig::getReturnCode() const { return returnCode; }

const std::string &LocationConfig::getReturnTarget() const {
  return returnTarget;
}

const std::string &LocationConfig::getUploadPath() const { return uploadPath; }

long LocationConfig::getClientMaxBodySize() const {
  return this->clientMaxBodySize;
}

const std::vector<std::string> &LocationConfig::getCgiPassExtensions() const {
  return this->cgiPassExtensions;
}
