#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <fstream>

#include "VirtualServer.hpp"
#include "Connection.hpp"

const std::string defaultConfigPath("./data/webserv.nginx.conf");

class Webserv
{
	private:
		int	_epoll_fd;
		std::vector<VirtualServer>	_servers;
		std::map<int, Connection >	_connections;
		std::string		_configPath;
		std::ifstream	_configFile;

		void	_openConfig(); 
		void	_parseConfig();

		VirtualServer&	_findCorrectServer(Connection const& conn);

		Webserv(void);

	public:
		Webserv(char *configPath);
		Webserv(const Webserv& src);
		Webserv& operator=(const Webserv& rhs);
		~Webserv();

		std::vector<VirtualServer>&	getServers(void);
		VirtualServer&				getServer (int idx);

		void	readConfig();
		void	initWebServer();
		void	run();

		bool	listenHandler(Connection & conn);
		bool	clientHandler(Connection & conn);

		//quick fix added for getting server root location -> deducted from the executable path
};

#endif // WEBSERV_HPP
