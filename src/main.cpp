#include "parser/config/ConfigParser.hpp"
#include "server/Server.hpp"

void logServerConfig(const ServerConfig &config, int serverIndex);

int main(int argc, char **argv) {
  if (argc > 2) {
    std::cerr
        << "Error: Too many arguments. Usage: ./webserv [config_file_path]"
        << std::endl;
    return 1;
  }

  std::string configPath = (argc == 2) ? argv[1] : "config/default.conf";
  std::cout << "--- Using configuration file: " << configPath << " ---\n";

  try {
    ConfigParser parser;
    parser.parse(configPath);
    parser.validateServerConfigs();

    const std::vector<ServerConfig> &servers = parser.getServerConfigs();
    std::cout << "--- Found " << servers.size()
              << " server configurations ---\n";

    for (size_t i = 0; i < servers.size(); ++i) {
      logServerConfig(servers[i], i);
    }

    Server s(parser);
    s.run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

void logServerConfig(const ServerConfig &config, int serverIndex) {
  std::cout << "\n=== Server " << serverIndex << " ===" << std::endl;

  const std::vector<std::string> &ips = config.getIPs();
  const std::vector<int> &ports = config.getPorts();
  for (size_t j = 0; j < ips.size(); ++j) {
    std::cout << "  Listen: IP = " << ips[j] << ", Port = " << ports[j]
              << std::endl;
  }

  const std::vector<std::string> &server_names = config.getServerNames();
  for (size_t j = 0; j < server_names.size(); ++j) {
    std::cout << "  Server Name: " << server_names[j] << std::endl;
  }

  std::cout << "  Root: " << config.getRoot() << std::endl;

  std::cout << "  Client Max Body Size: " << config.getClientMaxBodySize()
            << " bytes" << std::endl;

  const std::vector<std::string> &index_files = config.getIndexFiles();
  for (size_t j = 0; j < index_files.size(); ++j) {
    std::cout << "  Index: " << index_files[j] << std::endl;
  }

  const std::map<int, std::string> &error_pages = config.getErrorPages();
  for (std::map<int, std::string>::const_iterator it = error_pages.begin();
       it != error_pages.end(); ++it) {
    std::cout << "  Error Page: " << it->first << " -> " << it->second
              << std::endl;
  }
}
