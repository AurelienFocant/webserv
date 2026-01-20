#include "Webserv.hpp"
#include "ConfigNode.hpp"
#include "ConfigParser.hpp"
#include "ConfigBuilder.hpp"

#include <iostream>
#include <fstream>
#include <sys/epoll.h>

#define MAX_EVENTS 1024

Webserv::Webserv( void )
	: _configPath(defaultConfigPath)
{
}

Webserv::Webserv(char* configPath)
	: _configPath(defaultConfigPath)
{
	if (configPath) {
		_configPath = configPath;
	}
}

Webserv::Webserv( const Webserv& src )
{
	_configPath = src._configPath;
}

Webserv&	Webserv::operator=( const Webserv& rhs )
{
	if (this != &rhs) {
		_configPath = rhs._configPath;
	}
	return (*this);
}

Webserv::~Webserv( void )
{
	std::cout << "Webserv Object Destroyed" << std::endl;
}

void	Webserv::_openConfig(void)
{
	_configFile.open(_configPath.c_str());
	if (!_configFile.is_open())
		throw (std::runtime_error("Couldn't open config file"));
}

static std::string	_fileToString(std::ifstream & file)
{
	std::string	config;
	std::string	line;
	for (; std::getline(file, line); ) {
		config += line;
		config += "\n";
	}
	return (config);
}


void	Webserv::_parseConfig(void)
{
	ConfigTokenizer	tokenizer(_fileToString(_configFile));
	tokenizer.scanTokens();

	ConfigParser	parser(tokenizer.getTokenVec());
	parser.parseConfig();

	// both tokenizer and parser are constructed with what they need,
	// but builder takes it as argument to main fct ?
	ConfigBuilder	builder;
	_servers = builder.build(parser.getRoot());
}

void	Webserv::readConfig()
{
	_openConfig();
	_parseConfig();
}

void	Webserv::initWebServer()
{
	_epoll_fd = epoll_create(1);
	if (_epoll_fd < 0)
		throw (std::runtime_error("epoll_create failed"));
	// ?? put epoll fd non blocking ?

	for (std::vector<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); it++) {

		int	listenSocket;
		if ((listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
			throw (std::runtime_error("listen socket opening failed"));

		int	enable = 1;
		if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
			throw (std::runtime_error("socket options failed"));
		// ?? see more options ?

		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(it->getPort());
		if (bind(listenSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
			throw (std::runtime_error("bind failed"));

		if (listen(listenSocket, SOMAXCONN) < 0)
			throw (std::runtime_error("listen failed"));
		// ?? listen options ?


		_connections[listenSocket] = Connection(listenSocket, &Webserv::listenHandler);
		// Connection &conn = _connections[listenSocket];
		// (this->*(conn.handler))(conn);


		struct epoll_event	ev_hints;
		ev_hints.events = EPOLLIN;
		ev_hints.data.fd = listenSocket;
		epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, listenSocket, &ev_hints);


		// set up signals
	}
}

void	Webserv::run()
{
	while (1) {
		// check g_signum

		struct epoll_event	ready_events[MAX_EVENTS];
		int					event_count;
		event_count = epoll_wait(_epoll_fd, ready_events, MAX_EVENTS, -1);
		if (event_count < 0)
			break ;

		for (int i = 0; i < event_count; i++) {

			std::map<int, Connection>::iterator it = _connections.find(ready_events[i].data.fd);				// find the right key
			if (it == _connections.end())
				continue ;

			Connection & currConn = it->second;	// currConn is the value of <key, value>
			(this->*currConn.handler)(currConn);
		}


		// LOOP ready_events
			// call Connection.handler();
			// --> listenhandler()
				// create()
			// --> clientHandler()
				// HTTP madness emoji fire

		// ?? connection to be closed
			// ?? epoll_ctl DELETE connection from epoll_wait
			// close(fd);
			// delete from _connections map()
	}
}

std::vector<VirtualServer>&	Webserv::getServers(void)
{
	return (_servers);
}

VirtualServer&	Webserv::getServer(int idx)
{
	return (_servers.at(idx));
}

bool	Webserv::listenHandler(Connection & conn)
{
	int clientSocket = accept(conn.fd, NULL, 0);
	if (clientSocket < 0)
		return (false);
	// ?????????? // should we stop everything or just skip this connection ?

	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
		close(clientSocket);
		return (false);
	}

	struct epoll_event ev_hints;
	ev_hints.events = EPOLLIN | EPOLLRDHUP;
	ev_hints.data.fd = clientSocket;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, clientSocket, &ev_hints) < 0) {
		close(clientSocket);
		return (false);
	}

	_connections[clientSocket] = Connection(clientSocket, &Webserv::clientHandler);
	return (true);
}

bool	Webserv::clientHandler(Connection & conn)
{
	(void) conn;
	std::cout << "CLIENT HANDLER!\n";
	return (true);
}
