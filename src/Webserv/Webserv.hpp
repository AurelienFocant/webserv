#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <fstream>

#include "VirtualServer.hpp"
#include "Connection.hpp"
#include "cgi.hpp"

const std::string defaultConfigPath("./data/webserv.nginx.conf");

typedef struct	s_info {

	s_info(Connection& conn, bool (Webserv::*handler)(Connection& conn)):connection(conn), handler(handler) {}
	Connection&	connection;
	bool   		(Webserv::*handler)(Connection & conn);
}				t_info;

class RequestHandler;

class Webserv
{
	private:
		int	_epoll_fd;
		std::vector<VirtualServer>	_servers;
		std::map<int, t_info>		_connections;
		std::string					_configPath;
		std::ifstream				_configFile;

		void	_openConfig(); 
		void	_parseConfig();

		bool	_checkForRdHup(Connection & conn);
		void	_closeConnection(Connection & conn);
		void	_closeStaleConnections(void);
		void	_closeStaleCgi(void);
		bool	_addFdToEpoll(int client_fd, int events, int flags);

		bool	_startCGIresponse(RequestHandler & reqHandl, Connection & conn);

		const VirtualServer&	_resolveVirtualServer(const Connection& conn);


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
		bool	clientInHandler(Connection & conn);
		bool	clientOutHandler(Connection & conn);
		bool	cgiInHandler(Connection& conn);
		bool	cgiOutHandler(Connection& conn);
		

		//quick fix added for getting server root location -> deducted from the executable path
};

#endif // WEBSERV_HPP
