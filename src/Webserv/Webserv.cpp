#include "Webserv.hpp"
#include "ConfigParser.hpp"
#include "ConfigBuilder.hpp"
#include "RequestHandler.hpp"


#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>

#define MAX_EVENTS	1024

int g_signum;

static	void sigintHandler(int num)
{
	g_signum = num;
}

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

	//previous
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


		_connections[listenSocket] = Connection(listenSocket, _epoll_fd, &Webserv::listenHandler);
		// Connection &conn = _connections[listenSocket];
		// (this->*(conn.handler))(conn);


		struct epoll_event	ev_hints;
		ev_hints.events = EPOLLIN;
		ev_hints.data.fd = listenSocket;
		epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, listenSocket, &ev_hints);


		// set up signals
		signal(SIGINT, sigintHandler);
	}
}

void	Webserv::run()
{
	// for (int i = 0; i < 20; i++) {
	// 	std::cout << i << std::endl;
	while (1) {
		if (g_signum == SIGINT)
			return ;

		struct epoll_event	ready_events[MAX_EVENTS];
		int					event_count;
		event_count = epoll_wait(_epoll_fd, ready_events, MAX_EVENTS, 200);
		if (event_count < 0 && errno == EINTR)
			return ;
		if (event_count < 0)
			throw (std::runtime_error("epoll failed"));


		for (int i = 0; i < event_count; i++) {

			std::map<int, Connection>::iterator it = _connections.find(ready_events[i].data.fd);				// find the right key
			if (it == _connections.end())
				continue ;

			Connection & currConn = it->second;	// currConn is the value of <key, value>
			currConn.setEvent(ready_events[i].events);
			(this->*currConn.handler)(currConn);
		}

		// check keepalive_timeout and keepalive_time

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
	int clientSocket = accept(conn.getFd(), NULL, 0);
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

	//_connections[clientSocket] = Connection(clientSocket, _epoll_fd, &Webserv::clientHandler);
	Connection	new_connection(clientSocket, _epoll_fd, &Webserv::clientHandler);
	_connections.insert(std::make_pair(clientSocket, new_connection));
	return (true);
}

std::string	_receiveLoop(int fd)
{
	// TO BE REDONE !! //

	int		bytes_read; 
	char	buf[BUFFER_SIZE];
	std::string	s;

	while ((bytes_read = recv(fd, &buf, BUFFER_SIZE, 0)) > 0) {
		s.append(buf, bytes_read);
	}
	return (s);
}


VirtualServer&	Webserv::_findCorrectServer(Request const& request)
{
	// TODO
	(void) request;
	return (getServer(0));
}

bool	Webserv::clientHandler(Connection & conn)
{
	// client close gracefully
	if (conn.getEvent() & EPOLLRDHUP) {
	}
	// error
	if (conn.getEvent() & EPOLLHUP || conn.getEvent() & EPOLLERR) {
	}



	if (conn.getEvent() & EPOLLIN) {

		std::string request_str = _receiveLoop(conn.getFd());

		// RequestParser request_parser(request_str);
		// conn.request = request_parser.parseRequest();
		if (request_str.size() != 0) {
			conn.request.addInput(request_str);
			conn.request.parseRequest();
		}

		if (conn.request.getCompleted()) {
			struct epoll_event	ev_hints;
			ev_hints.events = EPOLLOUT | EPOLLRDHUP;
			ev_hints.data.fd = conn.getFd();
			epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, conn.getFd(), &ev_hints);
		}
	}

	else if (conn.getEvent() & EPOLLOUT) {
		conn.virtual_server = _findCorrectServer(conn.request);
	
		RequestHandler	reqHandl(conn);
		reqHandl.handleRequest();
		conn.response.formatResponse();
		conn.sendResponse();
		conn.request.cleanRequest();


		// conn.response = reqHandl.handleRequest();
		// _sendResponse();
	}

	// update last_conn_timestamp;



	return (true);
}
