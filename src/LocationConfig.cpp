/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:49:25 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 17:14:15 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/LocationConfig.hpp"

LocationConfig::LocationConfig() : _autoindex(false), _clientMaxBodySize(-1) {}

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig &other) { *this = other; }

LocationConfig &LocationConfig::operator=(const LocationConfig &other) {
  if (this != &other) {
    _path = other._path;
    _root = other._root;
    _allowedMethods = other._allowedMethods;
    _autoindex = other._autoindex;
    _indexFiles = other._indexFiles;
    _return = other._return;
    _uploadPath = other._uploadPath;
    _cgiPass = other._cgiPass;
    _clientMaxBodySize = other._clientMaxBodySize;
  }
  return *this;
}

const std::string &LocationConfig::getPath() const { return _path; }
const std::string &LocationConfig::getRoot() const { return _root; }
const std::vector<std::string> &LocationConfig::getAllowedMethods() const {
  return _allowedMethods;
}
bool LocationConfig::isAutoIndex() const { return _autoindex; }
const std::vector<std::string> &LocationConfig::getIndexFiles() const {
  return _indexFiles;
}
const std::pair<int, std::string> &LocationConfig::getReturn() const {
  return _return;
}
const std::string &LocationConfig::getUploadPath() const { return _uploadPath; }
const std::map<std::string, std::string> &LocationConfig::getCgiPass() const {
  return _cgiPass;
}
long LocationConfig::getClientMaxBodySize() const { return _clientMaxBodySize; }

std::vector<std::string> &LocationConfig::getAllowedMethods() {
  return _allowedMethods;
}
std::vector<std::string> &LocationConfig::getIndexFiles() {
  return _indexFiles;
}

void LocationConfig::setPath(const std::string &path) { _path = path; }
void LocationConfig::setRoot(const std::string &root) { _root = root; }
void LocationConfig::addAllowedMethod(const std::string &method) {
  _allowedMethods.push_back(method);
}
void LocationConfig::setAutoIndex(bool autoindex) { _autoindex = autoindex; }
void LocationConfig::addIndexFile(const std::string &file) {
  _indexFiles.push_back(file);
}
void LocationConfig::setReturn(int code, const std::string &url) {
  _return = std::make_pair(code, url);
}
void LocationConfig::setUploadPath(const std::string &path) {
  _uploadPath = path;
}
void LocationConfig::addCgiPass(const std::string &extension,
                                const std::string &binary_path) {
  _cgiPass[extension] = binary_path;
}
void LocationConfig::setClientMaxBodySize(long size) {
  _clientMaxBodySize = size;
}