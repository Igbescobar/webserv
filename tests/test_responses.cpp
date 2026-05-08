#include "MockRequest.hpp"
#include "parser_config/ConfigParser.hpp"
#include "response/HttpResponse.hpp" // Changed from ResponseHandler
#include <iostream>

// Helper to simulate epoll chunked sending
void simulateEpollSend(HttpResponse &response, int chunkSize) {
  std::cout << "[Simulator] Starting to send response in chunks of "
            << chunkSize << " bytes.\n";
  int totalSent = 0;

  while (!response.empty()) {
    std::string buffer = response.getResponse();

    // Calculate how many bytes we can actually "send" this iteration
    int bytesToSend = chunkSize;
    if (buffer.length() < (size_t)chunkSize) {
      bytesToSend = buffer.length();
    }

    // "Send" the bytes (just printing info)
    std::cout << "  -> Sent " << bytesToSend << " bytes.\n";

    // Tell the response object to forget the bytes we just sent
    response.erase(bytesToSend);

    totalSent += bytesToSend;
  }

  std::cout << "[Simulator] Finished! Sent a total of " << totalSent
            << " bytes.\n";
  std::cout << "[Simulator] Response empty() status: "
            << (response.empty() ? "true" : "false") << "\n\n";
}

void runTest(const ServerConfig &config, const std::string &method,
             const std::string &uri) {
  std::cout << "========== Testing " << method << " " << uri << " ==========\n";
  MockRequest req(method, uri);

  // Using the new colleague API design
  HttpResponse response(config, req);

  std::cout << "-- Raw Complete String --\n";
  std::cout << response.getResponse() << "\n";
  std::cout << "-------------------------\n";

  // Simulate sending it over a slow socket connection (e.g. 50 bytes at a time)
  simulateEpollSend(response, 50);
}

// Test what happens when an early parsing error occurs (e.g., 400 Bad Request)
void runErrorTest(const ServerConfig &config, int errorCode) {
  std::cout << "========== Testing Early Error Code " << errorCode
            << " ==========\n";

  // Using the other colleague constructor for direct errors
  HttpResponse response(config, errorCode);

  std::cout << "-- Raw Complete String --\n";
  std::cout << response.getResponse() << "\n";
  std::cout << "-------------------------\n";

  simulateEpollSend(response, 20);
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

    // 1. Valid request to index
    runTest(config, "GET", "/");

    // 2. Test a known directory without trailing slash (Should return 301
    // Redirect)
    runTest(config, "GET", "/media/images");

    // 3. Test a non-existent file (Should return 404 with your custom error
    // page)
    runTest(config, "GET", "/does_not_exist.html");

    // 4. Test method not allowed for a location (If POST isn't allowed)
    runTest(config, "POST", "/");

    // 5. Explicit early error parsing simulation
    runErrorTest(config, 400);

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
  }
  return 0;
}