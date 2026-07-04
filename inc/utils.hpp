#pragma once

#include <iostream>
#include <map>

void setNonBlocking(int fd);
void setCloseOnExec(int fd);

template <typename M, typename K, typename V>
void deleteMapItem(std::map<K, V> &map, K key) {
  V valueToDelete = map[key];
  map.erase(key);
  delete valueToDelete;
}

template <typename K, typename V> void printMap(std::map<K, V> &map) {
  typename std::map<K, V>::const_iterator it;

  std::cout << "{" << std::endl;
  for (it = map.begin(); it != map.end(); ++it) {
    std::cout << "    " << it->first << " = " << it->second << std::endl;
  }
  std::cout << "}" << std::endl;
}
