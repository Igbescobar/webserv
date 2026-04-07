/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igngonza <igngonza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:11:08 by igngonza          #+#    #+#             */
/*   Updated: 2026/04/07 17:13:38 by igngonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <map>
#include <string>
#include <vector>

class LocationConfig {
public:
  LocationConfig();
  ~LocationConfig();
  LocationConfig(const LocationConfig &other);
  LocationConfig &operator=(const LocationConfig &other);

  const std::string &getPath() const;
  const std::string &getRoot() const;
  const std::vector<std::string> &getAllowedMethods() const;
  bool isAutoIndex() const;
  const std::vector<std::string> &getIndexFiles() const;
  const std::pair<int, std::string> &getReturn() const;
  const std::string &getUploadPath() const;
  const std::map<std::string, std::string> &getCgiPass() const;
  long getClientMaxBodySize() const;

  std::vector<std::string> &getAllowedMethods();
  std::vector<std::string> &getIndexFiles();

  void setPath(const std::string &path);
  void setRoot(const std::string &root);
  void addAllowedMethod(const std::string &method);
  void setAutoIndex(bool autoindex);
  void addIndexFile(const std::string &file);
  void setReturn(int code, const std::string &url);
  void setUploadPath(const std::string &path);
  void addCgiPass(const std::string &extension, const std::string &binary_path);
  void setClientMaxBodySize(long size);

private:
  std::string _path;
  std::string _root;
  std::vector<std::string> _allowedMethods;
  bool _autoindex;
  std::vector<std::string> _indexFiles;
  std::pair<int, std::string> _return;
  std::string _uploadPath;
  std::map<std::string, std::string> _cgiPass;
  long _clientMaxBodySize;
};

#endif