#pragma once

#include <map>

void setNonBlocking(int fd);
void setCloseOnExec(int fd);

template <typename M, typename K, typename V>
void deleteMapItem(std::map<K, V> &map, K key) {
  V valueToDelete = map[key];
  map.erase(key);
  delete valueToDelete;
}
