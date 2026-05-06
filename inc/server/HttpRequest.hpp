#pragma once

#include <string>

class HttpRequest {
  private:
    std::string buf;
  public:
    HttpRequest();

    void append(std::string chunk);

    bool isCompleted();
};
