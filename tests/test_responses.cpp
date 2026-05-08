#include "MockRequest.hpp"
#include "parser_config/ConfigParser.hpp"
#include "response/ResponseHandler.hpp"
#include <iostream>

void runTest(const ServerConfig &config, const std::string &method,
             const std::string &uri) {
  std::cout << "========== Testing " << method << " " << uri << " ==========\n";
  MockRequest req(method, uri);
  ResponseHandler handler(config, req);
  std::cout << handler.handle() << "\n\n";
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

    // 2. Test a known directory without trailing slash (Should return 301
    // Redirect)
    runTest(config, "GET", "/media/images");

    // 3. Test a non-existent file (Should return 404 with your custom error
    // page)
    runTest(config, "GET", "/does_not_exist.html");

    // 4. Test method not allowed for a location (If POST isn't allowed)
    runTest(config, "POST", "/");

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
  }
  return 0;
}