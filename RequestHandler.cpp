#include <iostream>
#include <map>
#include <string>
#include "src/Request.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


class Location 
{
	private:

	/* Private Attributes */
	std::string _name;
	std::string _root;
	std::string _alias;

	public:

	/* Constructors / Destructors */
	Location	();
	Location	(const Location& other);
	Location&	operator= (const Location& rhs);
	~Location	();

	/* Getters */
	std::string	getName() const {return _name;}
	std::string	getRoot() const {return _root;}
	std::string	getAliast() const {return _alias;}

	/* Setters */
	void	setName(const std::string& name) {_name = name;}
	void	setRoot(const std::string& root) {_root = root;}
	void	setAlias(const std::string& alias) {_alias = alias;}
};

Location::Location	() : _name(""), _root(""), _alias("") {}
Location::Location	(const Location& other) : _name(other._name), _root(other._root), _alias(other._alias) {}
Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs)
	{
		_name = rhs._name;
		_root = rhs._root;
		_alias = rhs._alias;
	}
	return *this;
}
Location::~Location	() {}


class ServerConfig 
{
	public:

	ServerConfig	();
	ServerConfig	(const ServerConfig& other);
	ServerConfig&	operator= (const ServerConfig& rhs );
	~ServerConfig	();

	std::string _root;
	std::map<std::string, Location> route;
	std::map<int, std::string> errors_pages;

/* 	std::string	getRoot() const {return _root;}
	std::string	getLocation() const {return _root;} */
};

ServerConfig::ServerConfig () {}
ServerConfig::~ServerConfig () {}


class RequestHandler
{
	private:

	/* Private Attributes */
	//const Request&		_request;
	//const ServerConfig& _config;
	std::string			_root; // default root from config
	std::string			_path; // request_uri sans query
	std::string			_fullPath; //defnitive path (after alias or override)
	//std::string			_index; //define file(s)to search when URI point to a dir
	//t_HttpCode;			_statusCode;
	int					_statusCode;

	/* Private Methods */
	void	extractPath();
	void	resolvePath();
	bool	validatePath();

	public:

	/* Constructors / Destructors */
	RequestHandler	();
	//RequestHandler	(const Request& request);
	~RequestHandler	();

	/* Getters */
	std::string	getRoot() const {return _root;}

	/* Public Methods */
	void		handleRequest();
	std::string	buildResponse();
};

/* REQUEST HANDLER CPP */

RequestHandler::RequestHandler() : _root("/www/html"), _path(""), _fullPath("")_statusCode(200) {}

/* RequestHandler::RequestHandler(const Request& request) : _request(request),  _root("/www/html"), _statusCode(request.getStatusCode()){} */


RequestHandler::~RequestHandler() {}


void RequestHandler::extractPath()
{
	//_path = _request.getRequestUri();
	_path = "/";

	size_t queryPos = _path.find("?");
	if (queryPos != std::string::npos)
		_path = _path.substr(0, queryPos);
	
	std::cout << "Path: " << _path << std::endl;

}

void RequestHandler::resolvePath()
{
	_fullPath = _root + _path;
	std::cout << "Full Path: " << _fullPath << std::endl;

}

bool RequestHandler::validatePath()
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
	{
		if (errno == ENOENT)
			_statusCode = NOT_FOUND;
		if (errno == EACCES)
			_statusCode = FORBIDDEN;
		else
			_statusCode = INTERNAL_SERVER_ERROR;
		return false;
	}

	// + verifier permissions

	return true;
}


void RequestHandler::handleRequest()
{
	if (_statusCode != OK)
		return
	extractPath();

	resolvePath();

	validatePath();
}

/* REQUEST HANDLER CPP ~ END */


int	main()
{
	std::string method = "GET";

	Location route_test;
	route_test.setName("/errors/");
	route_test.setRoot("/var/");
	route_test.setAlias("");

	ServerConfig server_test;
	server_test._root = "/www/html";
	server_test.route["/error/"] = route_test;

	RequestHandler r_handler;
	r_handler.handleRequest();


/* 	Tests permissions
	std::cout << "isFile: " << (isFile("mainCopy.cpp") ? "true" : "false") << std::endl; 
	std::cout << "isDir: " << (isDir("mainCopy.cpp") ? "true" : "false") << std::endl;
	std::cout << "isReadable: " << (isReadable("mainCopy.cpp") ? "true" : "false") << std::endl; 
	std::cout << "isWritable: " << (isWritable("mainCopy.cpp") ? "true" : "false") << std::endl; 
	std::cout << "isExecutable: " << (isExecutable("mainCopy.cpp") ? "true" : "false") << std::endl << std::endl;  */

	return 0;
}

/* //Integration main:
RequestHandler r_handler(currConn->request_message);
r_handler.handleRequest();
currConn->response = r_handler.buildResponse(); */