#ifndef SERVERCONFIGPARSER_HPP
#define SERVERCONFIGPARSER_HPP

#include "ServerConfig.hpp"

class ServerConfigParser {

	private:
	
	enum result
	{
		INCOMPLETE,
		COMPLETE,
		ERROR
	}
	public:
	ServerConfigParser();
	ServerConfigParser(const ServerConfigParser &other);
	ServerConfigParser &operator=(const ServerConfigParser &other);
	~ServerConfigPaser();

	const int	get
}
#endif
