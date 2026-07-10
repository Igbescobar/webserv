#include "request/HttpRequest.hpp"
#include "parser_config/ServerConfig.hpp"
#include <iostream>
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
  body = other.body;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  serverConfig = other.serverConfig;
  buf = other.buf;
  state = other.state;
  errorCode = other.errorCode;
  return *this;
}

void HttpRequest::checkRequestLine() {
  if (method != "GET" && method != "POST" && method != "DELETE" &&
      method != "HEAD") {
    errorCode = 501;
    state = ERROR;
    return;
  }
  if (version != "HTTP/1.0" && version != "HTTP/1.1") {
    errorCode = 505;
    state = ERROR;
    return;
  }
}

void HttpRequest::checkRequestHeaders() {
  if (getHeader("host").empty()) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  if (getHeader("host").find(' ') != std::string::npos) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  if ((method == "GET" || method == "DELETE") &&
      (!getHeader("content-length").empty() ||
       !getHeader("transfer-encoding").empty())) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  if (method == "POST" && getHeader("content-length").empty() &&
      getHeader("transfer-encoding").empty()) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  const std::string &cl = getHeader("content-length");
  for (size_t i = 0; i < cl.size(); i++)
    if (!isdigit(cl[i])) {
      errorCode = 400;
      state = ERROR;
      return;
    }
  checkEarlyBodySizeLimit();
}

void HttpRequest::parseRequestLineValues(const std::string &requestLine) {
  size_t first = requestLine.find(" ");
  if (first == std::string::npos) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  size_t second = requestLine.find(" ", first + 1);
  if (second == std::string::npos) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  method = requestLine.substr(0, first);
  uri = requestLine.substr(first + 1, second - first - 1);
  version = requestLine.substr(second + 1);
  checkRequestLine();
}

void HttpRequest::parseRequestLine() {
  size_t end = buf.find("\r\n");
  if (end == std::string::npos) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  parseRequestLineValues(buf.substr(0, end));
  headerStart = end + 2;
}

bool HttpRequest::isValidHeaderKey(const std::string &key) {

  if (key.empty() || key.find(' ') != std::string::npos ||
      key.find('\t') != std::string::npos)
    return false;
  for (size_t i = 0; i < key.size(); i++)
    if (!isalnum(key[i]) && key[i] != '-' && key[i] != '_')
      return false;
  return true;
}

std::string HttpRequest::toLowerCase(const std::string &str) {
  std::string result = str;
  for (size_t i = 0; i < result.size(); i++)
    result[i] = tolower(result[i]);
  return result;
}

std::string HttpRequest::trim(const std::string &str) {
  size_t end = str.find_last_not_of(" \t\r\n");
  if (end == std::string::npos)
    return "";
  return str.substr(0, end + 1);
}

void HttpRequest::parseHeaderLine(const std::string &headerLine) {
  if (headerLine == "\r\n" || headerLine.empty())
    return;
  size_t colonPos = headerLine.find(":");
  if (colonPos == std::string::npos) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  std::string key = headerLine.substr(0, colonPos);
  if (!isValidHeaderKey(key)) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  key = toLowerCase(key);
  size_t valueStart = headerLine.find_first_not_of(" \t", colonPos + 1);
  if (valueStart == std::string::npos) {
    this->headers[key] = "";
    return;
  }
  this->headers[key] = trim(headerLine.substr(valueStart));
}

void HttpRequest::parseHeaders() {
  size_t headerEnd = buf.find(DELIMETER);
  if (headerEnd == std::string::npos) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  if (headerStart > headerEnd) {
    errorCode = 400;
    state = ERROR;
    return;
  }
  while (headerStart < headerEnd) {
    size_t lineEnd = buf.find("\r\n", headerStart);
    if (lineEnd == std::string::npos || lineEnd > headerEnd)
      lineEnd = headerEnd;
    parseHeaderLine(buf.substr(headerStart, (lineEnd + 2) - headerStart));
    if (state == ERROR) {
      return;
    }
    headerStart = lineEnd + 2;
  }
  checkRequestHeaders();
}

void HttpRequest::parseChunkedBody(size_t pos) {
  while (pos < buf.size()) {
    size_t chunkSizeEnd = buf.find("\r\n", pos);
    if (chunkSizeEnd == std::string::npos) {
      state = ERROR;
      return;
    }
    char *endptr;
    size_t chunkSize =
        std::strtoul(buf.substr(pos, chunkSizeEnd - pos).c_str(), &endptr, 16);
    if (*endptr != '\0') {
      errorCode = 400;
      state = ERROR;
      return;
    }
    if (chunkSize == 0) {
      state = COMPLETE;
      return;
    }
    size_t dataStart = chunkSizeEnd + 2;
    body += buf.substr(dataStart, chunkSize);
    pos = dataStart + chunkSize + 2;
  }
}

void HttpRequest::parseBody() {
  size_t bodyStart = buf.find(DELIMETER) + 4;
  if (getHeader("transfer-encoding") == "chunked") {
    parseChunkedBody(bodyStart);
    return;
  }
  if (!getHeader("content-length").empty()) {
    size_t number = std::strtoul(getHeader("content-length").c_str(), NULL, 10);
    size_t available = buf.size() - bodyStart;
    if (available < number) {
      state = INCOMPLETE;
      return;
    }
    body = buf.substr(bodyStart, number);
    state = COMPLETE;
    return;
  }

  if (state == INCOMPLETE) {
    std::istringstream stream(buf);
    stream >> method >> uri;
  }

  state = COMPLETE;
}

bool HttpRequest::isBodyComplete() {
  if (getHeader("transfer-encoding") == "chunked") {
    return buf.find("0\r\n\r\n") != std::string::npos;
  }
  if (!getHeader("content-length").empty()) {
    size_t bodyStart = buf.find(DELIMETER) + 4;
    size_t length = std::strtoul(getHeader("content-length").c_str(), NULL, 10);
    return buf.size() - bodyStart >= length;
  }
  return true;
}

void HttpRequest::append(const std::string &chunk) {
  buf += chunk;
  print();
  size_t pos = buf.find("\r\n");
  size_t pos1 = buf.find(DELIMETER);
  if (pos != std::string::npos && getVersion().empty()) {
    parseRequestLine();
    if (state == ERROR) {
      print();
      return;
    }
  }
  if (pos1 != std::string::npos && getHeaders().empty()) {
    parseHeaders();
    if (state == ERROR) {
      print();
      return;
    }
  }
  if (!getHeaders().empty()) {
    checkOngoingBodySizeLimit();
    if (state == ERROR) {
      print();
      return;
    }
  }
  if (!getHeaders().empty() && isBodyComplete()) {
    parseBody();
  }
  print();
}

long HttpRequest::getBodySizeLimit() const {
  const LocationConfig *loc = serverConfig.resolveLocation(this->getUri());
  return loc ? loc->getClientMaxBodySize()
             : serverConfig.getClientMaxBodySize();
}

void HttpRequest::checkEarlyBodySizeLimit() {
  const std::string &cl = getHeader("content-length");
  if (cl.empty())
    return;

  long limit = getBodySizeLimit();
  if (limit > -1) {
    long contentLength = std::strtol(cl.c_str(), NULL, 10);
    if (contentLength > limit) {
      errorCode = 413;
      state = ERROR;
    }
  }
}

void HttpRequest::checkOngoingBodySizeLimit() {

  long limit = getBodySizeLimit();
  if (limit > -1) {
    size_t bodyStart = buf.find(DELIMETER);
    if (bodyStart != std::string::npos) {
      size_t currentBodySize = buf.size() - (bodyStart + 4);
      if (currentBodySize > static_cast<size_t>(limit)) {
        errorCode = 413;
        state = ERROR;
      }
    }
  }
}

void HttpRequest::print() const {
  // std::cout << "=== HttpRequest ===\n";
  // std::cout << "State:      "
  //           << (state == INCOMPLETE ? "INCOMPLETE"
  //               : state == COMPLETE ? "COMPLETE"
  //                                   : "ERROR")
  //           << "\n";
  // std::cout << "ErrorCode:  " << errorCode << "\n";
  // std::cout << "Method:     " << method << "\n";
  // std::cout << "URI:        " << uri << "\n";
  // std::cout << "Version:    " << version << "\n";
  // std::cout << "Headers:\n";
  // for (std::map<std::string, std::string>::const_iterator it =
  // headers.begin();
  //      it != headers.end(); ++it)
  //   std::cout << "  " << it->first << ": " << it->second << "\n";
  // std::cout << "Body:       " << body << "\n";
  // std::cout << "Buf size:   " << buf.size() << "\n";
}

const std::string &HttpRequest::getMethod() const { return this->method; }
const std::string &HttpRequest::getUri() const { return this->uri; }
const std::string &HttpRequest::getVersion() const { return this->version; }
const std::string &HttpRequest::getBody() const { return this->body; }
const std::string &HttpRequest::getHeader(const std::string &key) const {
  static const std::string empty = "";
  std::map<std::string, std::string>::const_iterator it = headers.find(key);
  if (it == headers.end()) {
    return empty;
  }
  return it->second;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const {
  return this->headers;
}

const ServerConfig &HttpRequest::getServerConfig() const {
  return serverConfig;
}

t_state HttpRequest::getState() const { return state; }

int HttpRequest::getErrorCode() const { return errorCode; }

const std::string &HttpRequest::getRequest() const { return buf; }
