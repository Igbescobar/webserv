#include "response/HttpResponse.hpp"
#include "response/ResponseHandler.hpp"
#include <sstream>

HttpResponse::HttpResponse()
    : version("HTTP/1.0"), statusCode(200), statusMessage("OK"),
      isBuilt(false) {}

HttpResponse::HttpResponse(const HttpResponse &other) { *this = other; }

HttpResponse &HttpResponse::operator=(const HttpResponse &other) {
  if (this != &other) {
    this->version = other.version;
    this->statusCode = other.statusCode;
    this->statusMessage = other.statusMessage;
    this->headers = other.headers;
    this->body = other.body;
    this->rawResponse = other.rawResponse;
    this->isBuilt = other.isBuilt;
  }
  return *this;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusMessage(int code) {
  switch (code) {
  case 200:
    this->statusMessage = "OK";
    break;
  case 201:
    this->statusMessage = "Created";
    break;
  case 204:
    this->statusMessage = "No Content";
    break;
  case 301:
    this->statusMessage = "Moved Permanently";
    break;
  case 400:
    this->statusMessage = "Bad Request";
    break;
  case 403:
    this->statusMessage = "Forbidden";
    break;
  case 404:
    this->statusMessage = "Not Found";
    break;
  case 405:
    this->statusMessage = "Method Not Allowed";
    break;
  case 413:
    this->statusMessage = "Payload Too Large";
    break;
  case 500:
    this->statusMessage = "Internal Server Error";
    break;
  case 501:
    this->statusMessage = "Not Implemented";
    break;
  case 505:
    this->statusMessage = "HTTP Version Not Supported";
    break;
  default:
    this->statusMessage = "Unknown";
    break;
  }
}

void HttpResponse::setStatusCode(int code) {
  this->statusCode = code;
  setStatusMessage(code);
  this->isBuilt = false;
}

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
  this->headers[key] = value;
  this->isBuilt = false;
}

void HttpResponse::setBody(const std::string &body) {
  this->body = body;

  std::stringstream ss;
  ss << this->body.length();
  setHeader("Content-Length", ss.str());

  this->isBuilt = false;
}

std::string HttpResponse::toString() const {
  std::stringstream responseStream;

  responseStream << this->version << " " << this->statusCode << " "
                 << this->statusMessage << "\r\n";

  std::map<std::string, std::string>::const_iterator it;
  for (it = this->headers.begin(); it != this->headers.end(); it++)
    responseStream << it->first << ": " << it->second << "\r\n";

  responseStream << "\r\n";

  responseStream << this->body;

  return responseStream.str();
}

std::string HttpResponse::getResponse() {
  if (!this->isBuilt) {
    this->rawResponse = this->toString();
    this->isBuilt = true;
  }
  return this->rawResponse;
}

void HttpResponse::clearBody() {
  this->body.clear();
  this->isBuilt = false;
}
