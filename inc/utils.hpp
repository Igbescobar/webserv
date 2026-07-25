#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

void setNonBlocking(int fd);
void setCloseOnExec(int fd);

template <typename M, typename K, typename V>
void deleteMapItem(std::map<K, V> &map, K key) {
  V valueToDelete = map[key];
  map.erase(key);
  delete valueToDelete;
}

template <typename K, typename V> void printMap(const std::map<K, V> &map) {
  typename std::map<K, V>::const_iterator it;

  std::cerr << "{" << std::endl;
  for (it = map.begin(); it != map.end(); ++it) {
    std::cerr << "    " << it->first << " = " << it->second << std::endl;
  }
  std::cerr << "}" << std::endl;
}

template <typename T> void printVector(const std::vector<T> &v) {
  typename std::vector<T>::const_iterator it;

  std::cerr << "[" << std::endl;
  for (it = v.begin(); it != v.end(); ++it) {
    if (*it == NULL)
      std::cerr << "    " << "NULL" << std::endl;
    else
      std::cerr << "    " << *it << std::endl;
  }
  std::cerr << "]" << std::endl;
}

template <typename T> std::string toString(const T &value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}
