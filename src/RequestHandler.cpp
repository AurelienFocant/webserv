
#include "RequestHandler.hpp"

/* ////////////REQUEST HANDLER////////////////// */


//RequestHandler::RequestHandler() : _root("/www/html"), _path(""), _fullPath(""), _statusCode(200) {}

RequestHandler::RequestHandler(const Request& request) 
	: _request(request)
	, _root("/www/html")
	, _statusCode(request.getStatusCode())
	, _hasError(false)
{
	initRoutes();
	printRoutes();
}

RequestHandler::~RequestHandler() {}


bool RequestHandler::extractPath()
{
	if (_request.getRequestUri().empty() || _request.getRequestUri().at(0) != '/')
	{
		_statusCode = BAD_REQUEST;
		return false;
	}

	_path = _request.getRequestUri();
	std::cout << "RequestUri: " << _request.getRequestUri() << std::endl;
	std::cout << "PATH: " << _path << std::endl;

	size_t queryPos = _path.find("?");
	if (queryPos != std::string::npos)
		_path = _path.substr(0, queryPos);
	
	return true;
}

bool RequestHandler::resolvePath()
{
	if (_root.empty() || _path.empty())
	{
		_statusCode = BAD_REQUEST;
		return false;
	}

	_fullPath = _root + _path;
	std::cout << "Full Path: " << _fullPath << std::endl;

	return true;

}

bool RequestHandler::validatePath()
{
	struct stat statBuf;
	if (stat(_fullPath.c_str(), &statBuf) != 0)
	{
		if (errno == ENOENT)
			_statusCode = NOT_FOUND;
		else if (errno == EACCES)
			_statusCode = FORBIDDEN;
		else
			_statusCode = INTERNAL_SERVER_ERROR;
		return false;
	}

	_isDirectory = S_ISDIR(statBuf.st_mode);

	return true;
}

bool RequestHandler::processMethods()
{
	switch(_request.getMethod())
	{
		case GET:
			processGetMethod();
			break ;
/* 		case POST:
			processPostMethod();
			break ;
		case DELETE:
			processDeleteMethod(); */
			break ;
		default: 
			_statusCode = METHOD_NOT_ALLOWED; // ? 
			return false;
	}
	return true;
}

void	RequestHandler::processGetMethod()
{
	size_t		file_size;
	std::string content_type ;

	if (_isDirectory)
	{
		if (!resolveIndex())
			return;
	}

	int	fd = openReadFile(_fullPath);
	if (fd < 0)
		return;

	file_size = fileSize(_fullPath); // enregistrer dans Response
	content_type = "type"; // a implementer

/* 	
	// RESPONSABILITE de Connection -> Send Response au fur et a mesure

	char buffer[8192]; //8KB optimal?
	ssize_t bytesRead;

	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
		response_body.append(buffer, bytesRead);
	close(fd);
	if (bytesRead < 0)
	{
		_statusCode = INTERNAL_SERVER_ERROR;
		return;
	} */


	_statusCode = OK;
}

void RequestHandler::handleRequest()
{
	if (!extractPath() || !resolvePath() ||
		!validatePath() || !processMethods())
	{
		_hasError = true;
		return;
	}

	_statusCode = OK;
}

int RequestHandler::openReadFile(const std::string& path)
{
	int	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		switch (errno) {
		case EACCES:
			_statusCode = FORBIDDEN;
			break;
		case ENOENT:
			_statusCode = NOT_FOUND;
			break;
		default:
			_statusCode = INTERNAL_SERVER_ERROR;
			break;
		}
	}
	return fd;
}

int RequestHandler::openWriteFile(const std::string& path)
{
	int	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); // CHECK FLAGS -> APPEND?
	if (fd < 0)
	{
		switch (errno) {
		case EACCES:
			_statusCode = FORBIDDEN;
			break;
		case ENOENT:
			_statusCode = NOT_FOUND;
			break;
		default:
			_statusCode = INTERNAL_SERVER_ERROR;
			break;
		}
	}
	return fd;
}

bool RequestHandler::resolveIndex()
{
	return true;
}

bool RequestHandler::isDirectory(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return S_ISDIR(statBuf.st_mode);
}

size_t RequestHandler::fileSize(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return statBuf.st_size;
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
	imageLoc.setRoot("");
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
