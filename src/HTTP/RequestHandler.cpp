
#include "RequestHandler.hpp"

/* ////////////REQUEST HANDLER////////////////// */


/* RequestHandler::RequestHandler()
	: _root("/www/html")
	, _request_path("")
	, _full_path("")
	, _matched_location(NULL)
	, _is_directory(false)
	, _status_code(OK)
	, _has_error(false)
{} */

RequestHandler::RequestHandler(const Request& request, Response& response) 
	: _request(request)
	, _response(response)
	, _root("/www/html")
	, _request_path("")
	, _full_path("")
	, _matched_location(NULL)
	, _is_directory(false)
	, _status_code(request.getStatusCode())
	, _has_error(false)
{
	initRoutes();
	printRoutes();
}

RequestHandler::~RequestHandler() {}


void	RequestHandler::handleRequest()
{
	if (_request.getStatusCode() != OK)
	{
		_status_code = _request.getStatusCode();
		_has_error = true;
		return;
	}

	if (_request.getStatusCode() != OK || !extractPath() || !resolvePath() || !processMethods())
	{
		_has_error = true;
		return;
	}

	_status_code = OK;
}

/* PATH PROCESSING */

bool	RequestHandler::extractPath()
{
	if (_request.getRequestUri().empty() || _request.getRequestUri().at(0) != '/')
	{
		_status_code = BAD_REQUEST;
		return false;
	}

	_request_path = _request.getRequestUri();
	std::cout << "[DEBUG] RequestUri: " << _request.getRequestUri() << std::endl;
	std::cout << "[DEBUG] Path " << _request_path << std::endl;

	size_t queryPos = _request_path.find("?");
	if (queryPos != std::string::npos)
		_request_path = _request_path.substr(0, queryPos);
	
	return true;
}

bool	RequestHandler::resolvePath()
{
	findLocation();

/* 	if (_matched_location && !_matched_location->getRedirect().empty())
	{
		handleRedirect(); //ex: location /old_page { return 301 /new_page;} ou error_pages
		return true;
	} */
 
	if (_matched_location)
	{
		// Alias prioritaire sur root
		if (!_matched_location->getAlias().empty())
		{
			std::string	location_path = _matched_location->getName();
			std::string	remaining_path = _request_path.substr(location_path.length());
			_full_path = _matched_location->getAlias() + remaining_path;
		}
		else if (!_matched_location->getRoot().empty())
		{
			_root = _matched_location->getRoot();
			_full_path = _root + _request_path;
		}
		else
		{
			_full_path = _root + _request_path;
		}
	}
	else
		_full_path = _root + _request_path;

	std::cout << "[DEBUG] Full Path: " << _full_path << std::endl;

	if (!validatePath())
		return false;

	return true;

}

void	RequestHandler::findLocation()
{
	size_t		longest_match = 0;

	for (std::map<std::string, Location>::iterator it = _routes.begin(); it != _routes.end(); it++)
	{
		const std::string&	route_path = it->first;
		const Location*		location = &(it->second);

		if (_request_path.find(route_path, 0) == 0)
		{
			if (route_path.length() > longest_match)
			{
				longest_match = route_path.length();
				_matched_location = location;
			}
		}
	}
}

bool	RequestHandler::validatePath()
{
	struct stat statBuf;

	if (stat(_full_path.c_str(), &statBuf) != 0)
	{
		if (errno == ENOENT)
			_status_code = NOT_FOUND;
		else if (errno == EACCES)
			_status_code = FORBIDDEN;
		else
			_status_code = INTERNAL_SERVER_ERROR;
		return false;
	}

	_is_directory = S_ISDIR(statBuf.st_mode);

	return true;
}

/* METHODS PROCESSING */

bool	RequestHandler::processMethods()
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
			_status_code = METHOD_NOT_ALLOWED; // ? 
			return false;
	}
	return true;
}

void	RequestHandler::processGetMethod()
{
	size_t		file_size;
	std::string content_type ;

	if (_is_directory)
	{
		if (!resolveIndex())
		{
			if (hasAutoIndex())
			{
				generateAutoIndex();
				_status_code = OK;
			}
			else
				_status_code = FORBIDDEN; //Directory listing forbidden
			return;
		}
	}

	int	fd = openReadFile(_full_path);
	if (fd < 0)
		return;

	file_size = fileSize(_full_path); // enregistrer dans Response
	(void) file_size;
	content_type = getContentType(_full_path);

/* 	_response.setStatusCode(OK);
	_response.setContentType(content_type);
	_response.setContentLenght(file_size);
	_response.setBodyFd(fd); */

/* 	
	// RESPONSABILITE de Connection -> Send Response au fur et a mesure

	char buffer[8192]; //8KB optimal?
	ssize_t bytesRead;

	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
		response_body.append(buffer, bytesRead);
	close(fd);
	if (bytesRead < 0)
	{
		_status_code = INTERNAL_SERVER_ERROR;
		return;
	} */
	_status_code = OK;
}

/* INDEX/DIRECTORY HANDLING */

bool	RequestHandler::resolveIndex()
{
	std::vector<std::string>	index; //
	index.push_back("index.html"); //
	index.push_back("index.php"); //see storing type in ServerConfig

	if (index.empty())
		return false;

	std::string	dir_path = _full_path;
	if	(dir_path[dir_path.length() -1] != '/')
		dir_path += "/";

	for (size_t i = 0; i < index.size(); i++)
	{
		std::string test_path = dir_path + index[i];
		if (access(test_path.c_str(), R_OK) == 0)
		{
			_full_path = test_path;
			_is_directory = false;
			std::cout << "[DEBUG] index found: " << _full_path << std::endl;
			return true;
		}
	}
	return false;
}

bool	RequestHandler::hasAutoIndex()
{
	if (_matched_location)
		return (_matched_location->getAutoIndex());
	// ! a completer: autoindex pourrait aussi se trouver dans server
	return false;
}

void	RequestHandler::generateAutoIndex()
{
	// a implementer
	return;
}

/* UTILS */

bool	RequestHandler::isDirectory(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return false;
	return S_ISDIR(statBuf.st_mode);
}

std::string RequestHandler::getContentType(const std::string& path)
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "application/octet-stream";  // Default ? 
    }
    
    std::string ext = path.substr(dotPos + 1);
    
    if (ext == "html" || ext == "htm")return "text/html";
    else if (ext == "css") return "text/css";
    else if (ext == "js") return "application/javascript";
    else if (ext == "json") return "application/json";
    else if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    else if (ext == "png") return "image/png";
    else if (ext == "gif") return "image/gif";
    else if (ext == "txt") return "text/plain";
    else if (ext == "pdf") return "application/pdf";
    
    return "application/octet-stream";
}

/* FILE OPERATIONS */

int	RequestHandler::openReadFile(const std::string& path)
{
	int	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		switch (errno) {
		case EACCES:
			_status_code = FORBIDDEN;
			break;
		case ENOENT:
			_status_code = NOT_FOUND;
			break;
		default:
			_status_code = INTERNAL_SERVER_ERROR;
			break;
		}
	}
	return fd;
}

int	RequestHandler::openWriteFile(const std::string& path)
{
	int	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); // CHECK FLAGS -> APPEND?
	if (fd < 0)
	{
		switch (errno) {
		case EACCES:
			_status_code = FORBIDDEN;
			break;
		case ENOENT:
			_status_code = NOT_FOUND;
			break;
		default:
			_status_code = INTERNAL_SERVER_ERROR;
			break;
		}
	}
	return fd;
}

size_t RequestHandler::fileSize(const std::string& path)
{
	struct stat statBuf;
	if (stat(path.c_str(), &statBuf) != 0)
		return 0;
	return statBuf.st_size;
}

/* TESTS/DEBUG */

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

Location::Location	()
	: _name(""),
	_root(""),
	_alias(""),
	_index(),
	_autoindex(false)
{}

Location::Location	(const Location& other)
	: _name(other._name),
	_root(other._root),
	_alias(other._alias),
	_index(other._index),
	_autoindex(other._autoindex)
{}

Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs)
	{
		_name = rhs._name;
		_root = rhs._root;
		_alias = rhs._alias;
		_index = rhs._index;
		_autoindex = rhs._autoindex;
	}
	return *this;
}

Location::~Location	() {}
