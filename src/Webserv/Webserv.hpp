#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <fstream>

#include "VirtualServer.hpp"

const std::string defaultConfigPath("./data/webserv.nginx.conf");

class Webserv
{
	private:
		std::vector<VirtualServer> _servers;
		std::string		_configPath;
		std::ifstream	_configFile;

		void	_openConfig(); 
		void	_parseConfig();


	public:
		Webserv(void);
		Webserv(char *configPath);
		Webserv(const Webserv& src);
		Webserv& operator=(const Webserv& rhs);
		~Webserv();

		std::vector<VirtualServer>	getServers(void);
		void	readConfig();
		void	initWebServer();
};

#endif // WEBSERV_HPP
