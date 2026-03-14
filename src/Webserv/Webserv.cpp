#include "Webserv.hpp"
#include "ConfigParser.hpp"
#include "ConfigBuilder.hpp"
#include "RequestHandler.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <algorithm>

#define LISTEN_SOCK	0
#define MAX_EVENTS	1024

int g_signum;

/*static	void sigintHandler(int num)
{
	g_signum = num;
}
*/
// Config parsing and reading
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

static	bool	_isListenSocket(Connection const& conn)
{
		if (conn.getFirstConnTime() == LISTEN_SOCK)
			return (true);
		return (false);
}

void	Webserv::_closeConnection(Connection & conn)
{
	int	fd = conn.getFd();

	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	_connections.erase(fd);
	close(fd);
}

void	Webserv::_closeStaleConnections(void)
{
	std::map<int, t_info>::iterator	it;

	for (it = _connections.begin(); it != _connections.end(); it++) {
		Connection& conn = it->second.connection;

		if (_isListenSocket(conn))
			continue ;
		if (conn.conn_closed)
			return (_closeConnection(conn));
		// what about if its the very first connection and doesnt have any virtual server ?
		// if (conn.hasTimedOut())
		// 	return (_closeConnection(conn));
	}
}

void	Webserv::_closeStaleCgi(void)
{
	std::map<int, t_info>::iterator	it;

	for (it = _connections.begin(); it != _connections.end(); it++) {
		Connection& conn = it->second.connection;

		if (_isListenSocket(conn))
			continue ;
		if (conn.hasCgiTimedOut())
			if (!waitpid(conn.child_pid, NULL, WNOHANG)) {
				if (!kill(conn.child_pid, SIGKILL))
					perror("Kill child:");
				conn.request_handler.setRequestToComplete();
				conn.request_handler.getResponse().setState(Response::READY);
				conn.request_handler.getResponse().setStatusCode(GATEWAY_TIMEOUT);
				resp::prepareResponse(conn.request_handler.getResponse(), conn.request_handler.getRequest(), conn.request_handler.getVirtualServer()->getErrorPages());
			}
	}
	return ;
}

const VirtualServer&	Webserv::_resolveVirtualServer(const Connection& conn)
{
	sockaddr_in addr;
	socklen_t len = sizeof(addr);

	//get local port
	if (getsockname(conn.getFd(), (sockaddr*) &addr, &len) < 0)
		throw std::runtime_error("getsockname failed"); // add a catch block in clientOutHandler to avoid crashing the server?
														// only close the compromised connection

	unsigned int port = ntohs(addr.sin_port);

	std::string	host = conn.request_handler.getRequest().getHeaderValues("host")[0];

	size_t	colon = host.find(':');
	if (colon != std::string::npos)
		host = host.substr(0, colon);

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].getPort() == port && _servers[i].getServName() == host)
				return _servers[i];
	}

	for (size_t i = 0; i < _servers.size(); i++)
	{
		if (_servers[i].getPort() == port)
			return _servers[i];
	}

	std::cerr << "[ERROR] No server found for port" << port << "falling back to default server" << std::endl;
	return (getServer(0)); // return first server as default server
}

bool	Webserv::_addFdToEpoll(int client_fd, int events, int flags)
{
	struct epoll_event	ev_hints;
	ev_hints.events = events;
	ev_hints.data.fd = client_fd;
	if (epoll_ctl(_epoll_fd, flags, client_fd, &ev_hints) < 0) {
		return (false);
	};
	return (true);
}


// Setting up listening sockets
void	Webserv::initWebServer()
{
	_epoll_fd = epoll_create(1);
	if (_epoll_fd < 0)
		throw (std::runtime_error("epoll_create failed"));
	// ?? put epoll fd non blocking ?

	std::vector<int>	ports;
	for (std::vector<VirtualServer>::iterator it = _servers.begin(); it != _servers.end(); it++) {
		int	port = it->getPort();

		if (std::find(ports.begin(), ports.end(), port) != ports.end())
			return ;
		ports.push_back(port);

		int	listenSocket;
		if ((listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)) < 0)
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
		{
			perror("bind");
			throw (std::runtime_error("bind failed"));
		}

		if (listen(listenSocket, SOMAXCONN) < 0)
			throw (std::runtime_error("listen failed"));
		// ?? listen options ?

		Connection&	new_connection = *(new Connection(listenSocket, _epoll_fd, LISTEN_SOCK));
		t_info	info(new_connection, &Webserv::listenHandler);
		_connections.insert(std::make_pair(listenSocket, info));

		struct epoll_event	ev_hints;
		ev_hints.events = EPOLLIN;
		ev_hints.data.fd = listenSocket;
		epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, listenSocket, &ev_hints);

		// signal(SIGINT, sigintHandler);
		signal(SIGPIPE, SIG_IGN);
	}
}

bool	Webserv::_checkForRdHup(Connection & conn)
{
			if (conn.getEvent().events & EPOLLRDHUP) {
				conn.conn_closed = true;
				return (false);
			}

			if (conn.getEvent().events & EPOLLERR || conn.getEvent().events & EPOLLHUP) {
				if (conn.getEvent().data.fd == conn.getFd()) {
					std::cout << "[Error] HUP or ERR" <<std::endl; 
					conn.conn_closed = true;
				}
				return (false);
			}

			return (true);
}

// Main loop
void	Webserv::run()
{
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

			std::map<int, t_info>::iterator it = _connections.find(ready_events[i].data.fd);				// find the right key
			if (it == _connections.end())
				continue ;

			Connection & currConn = it->second.connection;	// currConn is the value of <key, value>
			currConn.setEvent(ready_events[i]);

			_checkForRdHup(currConn);
			(this->*(it->second.handler))(currConn);
			currConn.setLastConnTime(std::time(NULL));
		}

		_closeStaleConnections();
		_closeStaleCgi();
		//usleep(100);--> cheat ?
	}
}

bool	Webserv::listenHandler(Connection & conn)
{
	int clientSocket = accept(conn.getFd(), NULL, 0);
	if (clientSocket < 0)
		return (false);

	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | SOCK_CLOEXEC) < 0) {
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

	Connection& new_connection = *(new Connection(clientSocket, _epoll_fd, std::time(NULL)));
	t_info	info(new_connection, &Webserv::clientInHandler);
	_connections.insert(std::make_pair(clientSocket, info));
	return (true);
}

bool	Webserv::clientInHandler(Connection & conn)
{
	RequestHandler& reqHandler = conn.request_handler;
	const Request& request = conn.request_handler.getRequest();
	
	if (conn.getEvent().events & EPOLLIN) {

		std::string request_str = conn.receive();
		if (!request_str.size()) {
			return (false);
		}

		reqHandler.processRequest(request_str);

		if (request.getState() > PARSED && reqHandler.getVirtualServer() == NULL)
		{
			reqHandler.setVirtualServer(_resolveVirtualServer(conn));
			reqHandler.findLocation();
		}

		if (reqHandler.getVirtualServer() != NULL)
		{
			
			reqHandler.setRoot(reqHandler.getVirtualServer()->getRoot());
			if (!reqHandler.isAllowedMethod())
				reqHandler.setRequestToComplete();
			if (!request.isCompleted())
				reqHandler.processBody();
		}

		if (request.isCompleted()) {
			if (!_addFdToEpoll(conn.getFd(), EPOLLOUT | EPOLLRDHUP, EPOLL_CTL_MOD))
				conn.conn_closed = true;
		
			_connections.find(conn.getFd())->second.handler = &Webserv::clientOutHandler;
		}
	}
	return (true);
}

bool	Webserv::clientOutHandler(Connection & conn)
{
	RequestHandler& reqHandler = conn.request_handler;
	Response& response = conn.request_handler.getResponse();
	
	if (conn.getEvent().events & EPOLLOUT) {
		if (response.isDefault()) {
			reqHandler.handleRequest();

			if (reqHandler.validCgiRequest()) {
				if (!_startCGIresponse(conn.request_handler, conn))
				{
					const VirtualServer* server = conn.request_handler.getVirtualServer();
					const Request& request = conn.request_handler.getRequest();
					response.setStatusCode(INTERNAL_SERVER_ERROR);
					resp::prepareResponse(response, request, server->getErrorPages());
				}
			}
		}

		if (response.getState() != Response::PROCESSING_CGI) {
			response.formatResponse();
			conn.sendResponse();
		}

		if (response.isDone()) {
			if (response.getHeader("Connection") == "close")
			{
				conn.conn_closed = true;
				return (false);
			}

			reqHandler.clean();

			if (!_addFdToEpoll(conn.getFd(), EPOLLIN | EPOLLRDHUP, EPOLL_CTL_MOD))
				conn.conn_closed = true;

			_connections.find(conn.getFd())->second.handler = &Webserv::clientInHandler;
		}

	}
	return (true);
}


// CGI HANDLERS
bool	Webserv::_startCGIresponse(RequestHandler& reqHandler, Connection& conn)
{
	Response& response = reqHandler.getResponse();

	if (!_addFdToEpoll(conn.getFd(), 0, EPOLL_CTL_MOD))
		conn.conn_closed = true;
	char **env = cgi::buildCgiEnv(reqHandler);
	if (!env)
		return (false);
	if (!cgi::execute(reqHandler, conn, env))
		return (false);

	///////ADD PIPES TO EPOLL + MAP
	if (!_addFdToEpoll(conn.cgi_fd[1], EPOLLOUT | EPOLLRDHUP, EPOLL_CTL_ADD)) {
		close(conn.cgi_fd[0]);
		close(conn.cgi_fd[1]);
		return (false);
	}

	t_info	info(conn, &Webserv::cgiInHandler);
	_connections.insert(std::make_pair(conn.cgi_fd[1], info));

	if (!_addFdToEpoll(conn.cgi_fd[0], EPOLLIN | EPOLLHUP, EPOLL_CTL_ADD)) {
		std::cerr << "[ERROR] Cannot add CGI stdout to epoll" << std::endl;
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, conn.cgi_fd[1], NULL);
		_connections.erase(conn.cgi_fd[1]);
		close(conn.cgi_fd[0]);
		close(conn.cgi_fd[1]);
		return false;
	}

	t_info info_out(conn, &Webserv::cgiOutHandler);
	_connections.insert(std::make_pair(conn.cgi_fd[0], info_out));
	//////////////////////////////////////////

	response.setState(Response::PROCESSING_CGI);
	conn.cgi_timeout = std::time(NULL);
	return (true);
}

bool	Webserv::cgiInHandler(Connection& conn)
{
	int bytes_sent = 0;

	conn.sendCgiContent(bytes_sent);
	if (bytes_sent < 0) {
		//Error do something;
		return (false);
	}

	if (conn.cgi_stdin_offset == conn.request_handler.getRequest().getContentLength()) {
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, conn.cgi_fd[1], NULL);
		_connections.erase(conn.cgi_fd[1]);
		close(conn.cgi_fd[1]);
		conn.cgi_fd[1] = -1;
		conn.cgi_stdin_offset = 0;
	}

	conn.cgi_timeout = std::time(NULL);
	return (true);
}

bool Webserv::cgiOutHandler(Connection& conn)
{
	Response& response = conn.request_handler.getResponse();
	const Request& request = conn.request_handler.getRequest();

    char buffer[32000];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_read = read(conn.cgi_fd[0], buffer, sizeof(buffer));

    if (bytes_read < 0) {
        return true;
    }
    else if (bytes_read == 0) {
        
		if (!_addFdToEpoll(conn.getFd(), EPOLLOUT | EPOLLRDHUP, EPOLL_CTL_MOD))
			conn.conn_closed = true;

        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, conn.cgi_fd[0], NULL);
        _connections.erase(conn.cgi_fd[0]);
        close(conn.cgi_fd[0]);

        int status;
        if (waitpid(conn.child_pid, &status, WNOHANG) == 0) {
            if (!kill(conn.child_pid, SIGKILL))
				perror("Kill child:");
        }
        
		if (!cgi::parseOutput(response))
		{
			response.setStatusCode(INTERNAL_SERVER_ERROR);
			return false;
		}
        resp::prepareResponse(response, request, conn.request_handler.getVirtualServer()->getErrorPages());
        response.setState(Response::READY);
    }
    else {
        response.addCgiBody(buffer, bytes_read);
        conn.cgi_timeout = std::time(NULL);
    }
    return true;
}

// Getters
std::vector<VirtualServer>&	Webserv::getServers(void)
{
	return (_servers);
}

VirtualServer&	Webserv::getServer(int idx)
{
	return (_servers.at(idx));
}

// Constructors and stuff
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
	for (std::map<int, t_info>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		delete (&(it->second.connection));
	}
	std::cout << "Webserv Object Destroyed" << std::endl;
}
