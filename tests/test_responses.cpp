#include "MockRequest.hpp"
#include "parser_config/ConfigParser.hpp"
#include "response/HttpResponse.hpp"
#include "response/ResponseHandler.hpp"
#include <iostream>

static void printResponse(HttpResponse &response) {
  // HttpResponse is now treated as an already-built response container.
  // Chunking/erase/empty are server responsibilities, not test
  // responsibilities.
  std::cout << "-- Raw Complete String --\n";
  std::cout << response.getResponse() << "\n";
  std::cout << "-------------------------\n\n";
}

static void runTest(const ServerConfig &config, const std::string &method,
                    const std::string &uri) {
  std::cout << "========== Testing " << method << " " << uri << " ==========\n";
  MockRequest req(method, uri);

  ResponseHandler handler(config, req);
  HttpResponse response = handler.handle();

  printResponse(response);
}

static void runErrorTest(const ServerConfig &config, int errorCode) {
  std::cout << "========== Testing Early Error Code " << errorCode
            << " ==========\n";

  ResponseHandler handler(config, errorCode);
  HttpResponse response = handler.handle();

  printResponse(response);
}
static void runAutoIndexTest(const ServerConfig &config) {
  std::cout << "========== Testing AutoIndex (GET /media/images/) ==========\n";

  // This assumes your config has a location with autoindex enabled for this
  // URI. If not, add one to config/default.conf, e.g.: location
  // /autoindex_test_dir/ { root <...>; autoindex on; }
  MockRequest req("GET", "/media/images/");

  ResponseHandler handler(config, req);
  HttpResponse response = handler.handle();

  printResponse(response);

  // Basic sanity check: autoindex response should contain HTML and "Index of"
  const std::string raw = response.getResponse();
  if (raw.find("200") == std::string::npos ||
      raw.find("Index of") == std::string::npos) {
    std::cerr
        << "[WARN] AutoIndex test did not look like a 200 directory listing.\n"
        << "       Check that autoindex is enabled for /autoindex_test_dir/ in "
           "config,\n"
        << "       and that the directory exists under the configured root.\n";
  }
}

int main() {
  try {
    ConfigParser parser;
    parser.parse("config/default.conf");
    std::vector<ServerConfig> servers = parser.getServerConfigs();

    if (servers.empty()) {
      std::cerr << "No servers found in config!\n";
      return 1;
    }

    ServerConfig &config = servers[0];

    runTest(config, "GET", "/");
    runTest(config, "GET", "/media/images");
    runTest(config, "GET", "/does_not_exist.html");
    runTest(config, "POST", "/");
    runErrorTest(config, 400);

    runAutoIndexTest(config);

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
  }
}
