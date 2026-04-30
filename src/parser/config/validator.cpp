#include "parser/config/ConfigParser.hpp"
#include <set>
#include <sstream>
#include <stdexcept>

void ConfigParser::validateHasValue(const std::string &directive) const {
  if (!hasMoreTokens() || isDelimiter(peekToken()[0])) {
    throw std::runtime_error("Directive '" + directive + "' requires a value.");
  }
}

void ConfigParser::validateOnlyOneValue(const std::string &directive) const {
  if (hasMoreTokens() && !isDelimiter(peekToken()[0])) {
    throw std::runtime_error("Directive '" + directive +
                             "' requires exactly one value.");
  }
}

void ConfigParser::validateNotDuplicate(const std::string &directive,
                                        bool isSet) const {
  if (isSet) {
    throw std::runtime_error("Duplicate '" + directive + "' directive found.");
  }
}

void ConfigParser::validateSemicolon() { consumeToken(";"); }

static std::runtime_error makeServerError(size_t serverIndex,
                                          const std::string &msg) {
  std::stringstream ss;
  ss << "Server " << serverIndex << ": " << msg;
  return std::runtime_error(ss.str());
}

void ConfigParser::validateHasServerBlocks() const {
  if (serverConfigs.empty()) {
    throw std::runtime_error(
        "Configuration file must contain at least one server block.");
  }
}

void ConfigParser::validateEachServerHasMandatoryDirectives() const {
  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    const ServerConfig &cfg = serverConfigs[i];

    if (cfg.getListens().empty()) {
      throw makeServerError(i, "missing required 'listen' directive.");
    }
    if (cfg.getRoot().empty()) {
      throw makeServerError(i, "missing required 'root' directive.");
    }
  }
}

void ConfigParser::validateNoDuplicateIpPortAcrossServers() const {
  std::set<std::pair<std::string, int> > used;

  for (size_t i = 0; i < serverConfigs.size(); ++i) {
    const ServerConfig &cfg = serverConfigs[i];
    const std::vector<std::string> &ips = cfg.getIPs();
    const std::vector<int> &ports = cfg.getPorts();

    if (ips.size() != ports.size()) {
      throw makeServerError(i,
                            "internal error: listen ip/port count mismatch.");
    }

    for (size_t j = 0; j < ips.size(); ++j) {
      std::pair<std::string, int> endpoint(ips[j], ports[j]);
      if (used.find(endpoint) != used.end()) {
        std::stringstream ss;
        ss << "duplicate listen endpoint '" << ips[j] << ":" << ports[j] << "'";
        throw makeServerError(i, ss.str());
      }
      used.insert(endpoint);
    }
  }
}

void ConfigParser::validateCgiPassExtension(const std::string &ext) {
  if (ext.size() < 2 || ext[0] != '.') {
    throw std::runtime_error("Invalid cgi_pass value: '" + ext +
                             "' (expected extension like '.php')");
  }
}