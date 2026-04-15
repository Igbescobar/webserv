#include "../inc/HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse()
    : version("HTTP/1.1"), statusCode(200), statusMessage("OK") {}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusMessage(int code) {
  switch (code) {
  case 200:
    this->statusMessage = "OK";
    break;
  case 201:
    this->statusMessage = "Created";
    break;
  case 400:
    this->statusMessage = "Bad Request";
    break;
  case 404:
    this->statusMessage = "Not Found";
    break;
  case 405:
    this->statusMessage = "Method Not Allowed";
    break;
  case 500:
    this->statusMessage = "Internal Server Error";
    break;
  default:
    this->statusMessage = "Unknown";
    break;
  }
}

void HttpResponse::setStatusCode(int code) {
  this->statusCode = code;
  setStatusMessage(code);
}

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
  this->headers[key] = value;
}

void HttpResponse::setBody(const std::string &body) {
  this->body = body;

  std::stringstream ss;
  ss << this->body.length();
  setHeader("Content-Length", ss.str());
}

std::string HttpResponse::toString() const {
  std::stringstream responseStream;

  responseStream << this->version << " " << this->statusCode << " "
                 << this->statusMessage << "\r\n";

  std::map<std::string, std::string>::const_iterator it;
  for (it = this->headers.begin(); it != this->headers.end(); it++)
    responseStream << it->first << ":" << it->second << "\r\n";

  responseStream << "\r\n";

  responseStream << this->body;

  return responseStream.str();
}