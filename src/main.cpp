#include "Parser.hpp"

int main() {
    try {
        Parser p;
        p.tokenizeFile("test.conf");

        const std::vector<std::string>& results = p.getTokens();
        std::cout << "Tokens found:" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "[" << i << "] -> " << results[i] << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
