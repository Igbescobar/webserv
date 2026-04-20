#include "../inc/ConfigParser.hpp"

int main() {
  try {
    ConfigParser parser;
    parser.parse("config/default.conf");

    const std::vector<ServerConfig> &servers = parser.getServerConfigs();
    for (size_t i = 0; i < servers.size(); ++i) {
      std::cout << "Server " << i << " listen values:\n";
      const std::vector<std::string> &listens = servers[i].getListen();
      for (size_t j = 0; j < listens.size(); ++j) {
        std::cout << "  [" << j << "] " << listens[j] << std::endl;
      }
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
