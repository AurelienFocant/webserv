
#include "RequestHandler.hpp"

/* ////////////REQUEST HANDLER////////////////// */


//RequestHandler::RequestHandler() : _root("/www/html"), _path(""), _fullPath(""), _statusCode(200) {}

RequestHandler::RequestHandler(const Request& request, Response& response) 
	: _request(request)
	, _response(response)
	, _root("/www/html")
	, _path("")
	, _fullPath("")
	, _matchedLocation(NULL)
	, _isDirectory(false)
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

void RequestHandler::findLocation()
{
	size_t		longest_match = 0;

	for (std::map<std::string, Location>::iterator it = _routes.begin(); it != _routes.end(); it++)
	{
		const std::string&	route_path = it->first;
		const Location*		location = &(it->second);

		if (_path.find(route_path, 0) == 0)
		{
			if (route_path.length() > longest_match)
			{
				longest_match = route_path.length();
				_matchedLocation = location;
			}
		}
	}
}

bool RequestHandler::resolvePath()
{
	//definir which hostname + if location
	// + test allowed methods (exists only in locations)

	findLocation();

	if (_matchedLocation)
	{
		if (!_matchedLocation->getAlias().empty())
		{
			std::string	location_path = _matchedLocation->getName();
			std::string	remaining_path = _path.substr(location_path.length());
			_fullPath = _matchedLocation->getAlias() + remaining_path;
		}
		else if (!_matchedLocation->getRoot().empty())
		{
			_root = _matchedLocation->getRoot();
			_fullPath = _root + _path;
		}
		else
		{
			_fullPath = _root + _path;
		}
	}
	else
		_fullPath = _root + _path;

/* 	if (root.empty() || _path.empty())
	{
		_statusCode = BAD_REQUEST;
		return false;
	} */


	std::cout << "Full Path: " << _fullPath << std::endl;

	if (!validatePath())
		return false;

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
	content_type = getContentType(_fullPath);

	(void) file_size;

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
	if (_request.getStatusCode() != OK || !extractPath() || !resolvePath() || !processMethods())
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
	std::vector<std::string>index;
	index.push_back("index.html");
	index.push_back("index.htm"); //see storing type in ServerConfig

	std::string				dirPath = _fullPath;
	bool					autoindex = false;

	if (index.empty())
		return false;
	for (size_t i = 0; i < index.size(); i++)
	{
		std::string testPath = dirPath + index[i];
		if (access(testPath.c_str(), R_OK) == 0)
		{
			_fullPath = testPath;
			return true;
		}
	}
	if (autoindex)
	{
		// NGINX will automatically generate and display a directory listing
		// adapt _fullPath;
		return true;
	}
	// _statusCode = ??
	return false;
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
		return 0;
	return statBuf.st_size;
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


