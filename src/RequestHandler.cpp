
#include "RequestHandler.hpp"

/* ////////////REQUEST HANDLER////////////////// */


//RequestHandler::RequestHandler() : _root("/www/html"), _path(""), _fullPath(""), _statusCode(200) {}

RequestHandler::RequestHandler(const Request& request) : _request(request),  _root("/www/html"), _statusCode(request.getStatusCode())
{
	initRoutes();
	printRoutes();
}

void RequestHandler::initRoutes()
{

	Location rootLoc;
	rootLoc.setName("/");
	rootLoc.setRoot("/var/www/html");
	rootLoc.setAlias("");
	_routes["/"] = rootLoc;

	Location imageLoc;
	imageLoc.setName("/images");
	imageLoc.setAlias("/var/www/assets/images");
	rootLoc.setRoot("");
	_routes["/images"] = imageLoc;
}

void	RequestHandler::printRoutes()
{
	std::map<std::string, Location>::iterator it;

	std::cout << "---------Print routes---------"<< std::endl;
	for (it = _routes.begin(); it != _routes.end(); it++)
	{
		std::cout << "Key: " << it->first
		<< "\nName-> " << it->second.getName()
		<< "\nRoot-> " << it->second.getRoot()
		<< "\nAlias-> " << it->second.getAlias() << std::endl;
	}
	std::cout << "---------------------------"<< std::endl;

}

RequestHandler::~RequestHandler() {}


void RequestHandler::extractPath()
{
	if (_request.getRequestUri().empty() || _request.getRequestUri().at(0) != '/')
	{
		std::cerr << "BAD URI: " << _request.getRequestUri() << std::endl;
		return;
	}

	_path = _request.getRequestUri();
	std::cout << "RequestUri: " << _request.getRequestUri() << std::endl;
	std::cout << "PATH: " << _path << std::endl;

	size_t queryPos = _path.find("?");
	if (queryPos != std::string::npos)
		_path = _path.substr(0, queryPos);
}

void RequestHandler::resolvePath()
{

	_fullPath = _root + _path;
	std::cout << "Full Path: " << _fullPath << std::endl;

}

bool RequestHandler::validatePath()
{
	struct stat statBuf;
	if (stat(_path.c_str(), &statBuf) != 0)
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
	extractPath();

	resolvePath();

	validatePath();
}

/* ///////////LOCATION/////////////////// */

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


/* int	main()
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


	Tests permissions
	std::cout << "isFile: " << (isFile("mainCopy.cpp") ? "true" : "false") << std::endl; 
	std::cout << "isDir: " << (isDir("mainCopy.cpp") ? "true" : "false") << std::endl;
	std::cout << "isReadable: " << (isReadable("mainCopy.cpp") ? "true" : "false") << std::endl; 
	std::cout << "isWritable: " << (isWritable("mainCopy.cpp") ? "true" : "false") << std::endl; 
	std::cout << "isExecutable: " << (isExecutable("mainCopy.cpp") ? "true" : "false") << std::endl << std::endl; 

	return 0;
} */

/* //Integration main:
RequestHandler r_handler(currConn->request_message);
r_handler.handleRequest();
currConn->response = r_handler.buildResponse(); */