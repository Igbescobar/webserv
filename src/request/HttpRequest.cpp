#include "request/HttpRequest.hpp"
#include "parser_config/ServerConfig.hpp"
#include <sstream>
#include <string>

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(ServerConfig serverConfig)
    : serverConfig(serverConfig), method("GET"), uri("/") {
  errorCode = -1;
  state = INCOMPLETE;
}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  method = other.method;
  uri = other.uri;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  method = other.method;
  uri = other.uri;
  return *this;
}

void HttpRequest::append(std::string chunk) {
  buf += chunk;
  updateState();
}

void HttpRequest::updateState() {
  size_t pos = buf.find(DELIMETER);
  if (pos == std::string::npos) {
    state = INCOMPLETE;
    return;
  }

  if (state == INCOMPLETE) {
    std::istringstream stream(buf);
    stream >> method >> uri;
  }

  state = COMPLETE;
}

const ServerConfig &HttpRequest::getServerConfig() const {
  return serverConfig;
}

t_state HttpRequest::getState() { return state; }

int HttpRequest::getErrorCode() { return errorCode; }

std::string HttpRequest::getRequest() { return buf; }

std::string HttpRequest::getUri() const { return uri; }

std::string HttpRequest::getMethod() const { return method; }

std::string HttpRequest::getBody() const {
  const std::string delim = "\r\n\r\n";
  size_t pos = buf.find(delim);
  if (pos == std::string::npos)
    return "";
  return buf.substr(pos + delim.size());
}

static std::string toLowerAscii(const std::string &s) {
  std::string out;
  out.reserve(s.size());
  for (size_t i = 0; i < s.size(); ++i)
    out += static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
  return out;
}

static std::string trimSpaces(const std::string &s) {
  size_t b = 0;
  while (b < s.size() &&
         (s[b] == ' ' || s[b] == '\t' || s[b] == '\r' || s[b] == '\n'))
    ++b;

  size_t e = s.size();
  while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\r' ||
                   s[e - 1] == '\n'))
    --e;

  return s.substr(b, e - b);
}

// Boilerplate header lookup for response testing.
// - Case-insensitive header name match
// - Returns "" if missing
// - Scans only the header section (before \r\n\r\n)
std::string HttpRequest::getHeader(const std::string &name) const {
  const std::string needle = toLowerAscii(name);

  const size_t headerEnd = buf.find(DELIMETER);
  const size_t scanEnd =
      (headerEnd == std::string::npos) ? buf.size() : headerEnd;

  // Skip request line
  size_t lineStart = buf.find("\r\n");
  if (lineStart == std::string::npos || lineStart >= scanEnd)
    return "";
  lineStart += 2;

  while (lineStart < scanEnd) {
    size_t lineEnd = buf.find("\r\n", lineStart);
    if (lineEnd == std::string::npos || lineEnd > scanEnd)
      lineEnd = scanEnd;

    if (lineEnd == lineStart)
      break;

    const std::string line = buf.substr(lineStart, lineEnd - lineStart);
    const size_t colon = line.find(':');
    if (colon != std::string::npos) {
      const std::string key = toLowerAscii(trimSpaces(line.substr(0, colon)));
      if (key == needle)
        return trimSpaces(line.substr(colon + 1));
    }

    lineStart = lineEnd + 2;
  }

  return "";
}
