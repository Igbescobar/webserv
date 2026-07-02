#include "response/CgiResponder.hpp"
#include "response/ResponseFactory.hpp"
#include <sstream>

HttpResponse CgiResponder::handle(const ServerConfig &config,
                                  const HttpRequest &req,
                                  const std::string &cgiOutput) {
  size_t sep = cgiOutput.find("\r\n\r\n");
  size_t sepLen = 4;
  if (sep == std::string::npos) {
    sep = cgiOutput.find("\n\n");
    sepLen = 2;
  }

  std::string cgiHeaders;
  std::string body;
  if (sep == std::string::npos) {
    body = cgiOutput;
  } else {
    cgiHeaders = cgiOutput.substr(0, sep);
    body = cgiOutput.substr(sep + sepLen);
  }

  HttpResponse resp = ResponseFactory::make(req, config, 200);

  applyCgiHeaders(resp, cgiHeaders);

  resp.setBody(body);
  return resp;
}

void CgiResponder::applyCgiHeaders(HttpResponse &resp,
                                   const std::string &cgiHeaders) {
  std::istringstream stream(cgiHeaders);
  std::string line;

  while (std::getline(stream, line)) {
    if (!line.empty() && line[line.size() - 1] == '\r')
      line.erase(line.size() - 1);
    if (line.empty())
      continue;

    size_t colon = line.find(':');
    if (colon == std::string::npos)
      continue;

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);
    size_t start = value.find_first_not_of(' ');
    if (start != std::string::npos)
      value = value.substr(start);

    if (key == "Status") {
      int code = 200;
      std::istringstream ss(value);
      ss >> code;
      resp.setStatusCode(code);
    } else {
      resp.setHeader(key, value);
    }
  }
}
