#include "RequestHandler.hpp"
#include "HtmlBuilder.hpp"

/* ////////////REQUEST HANDLER////////////////// */

RequestHandler::RequestHandler(Connection& currConn) 
	: _request(currConn.request)
	, _response(currConn.response)
	, _server(currConn.virtual_server)
	, _root(currConn.virtual_server.getRoot())
	, _request_path("")
	, _resolved_path("")
	, _matched_location(NULL)
	, _is_directory(false)
{
	_response.setStatusCode(_request.getStatusCode());
	//printRoutes();
}

RequestHandler::~RequestHandler() {}


void	RequestHandler::handleRequest()
{
	//std::cout << "Just here to use _response: " <<_response.SEND_HEADER << std::endl; 

	if (_response.getStatusCode() != OK)
		return;

	if (!extractPath() || !resolvePath() || !processMethods())
		return;

	_response.setStatusCode(OK);
}

/* PATH PROCESSING */

bool	RequestHandler::extractPath()
{
	if (_request.getRequestUri().empty() || _request.getRequestUri().at(0) != '/')
	{
		_response.setStatusCode(BAD_REQUEST);
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
			_resolved_path = _matched_location->getAlias() + remaining_path;
		}
		else if (!_matched_location->getRoot().empty())
		{
			_root = _matched_location->getRoot();
			_resolved_path = _root + _request_path;
		}
		else
		{
			_resolved_path = _root + _request_path;
		}
	}
	else
		_resolved_path = _root + _request_path;

	std::cout << "[DEBUG] Full Path: " << _resolved_path << std::endl;

	if (!validatePath())
		return false;

	return true;

}

void	RequestHandler::findLocation()
{
	size_t		longest_match = 0;

	for (std::map<std::string, Location>::const_iterator it = _server.getLocations().begin(); it != _server.getLocations().end(); it++)
	{
		const std::string&	route_path = it->first;
		const Location*		location = &(it->second);

		if (_request_path.find(route_path, 0) == 0)
		{
			size_t route_len = route_path.length();
			if (_request_path.length() == route_len || _request_path[route_len] == '/')
			{
				if (route_len > longest_match)
				{
					longest_match = route_path.length();
					_matched_location = location;
				}
			}
		}
	}
}

bool	RequestHandler::validatePath()
{
	struct stat statBuf;

	if (stat(_resolved_path.c_str(), &statBuf) != 0)
	{
		if (errno == ENOENT)
			 _response.setStatusCode(NOT_FOUND);
		else if (errno == EACCES)
			_response.setStatusCode(FORBIDDEN);
		else
			_response.setStatusCode(INTERNAL_SERVER_ERROR);
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
			_response.setStatusCode(METHOD_NOT_ALLOWED); // ? 
			return false;
	}
	return true;
}

void	RequestHandler::processGetMethod()
{
	if (_is_directory)
	{
		if	(_resolved_path[_resolved_path.length() -1] != '/') //Le serveur n'a pas le droit de modifier l'url en "silence. soit redirect /dir/ soit 404"
		{
			_response.setStatusCode(MOVED_PERMANENTLY);
			// set response header Location: request_path + "/";
			return;
		}
		if (!resolveIndex())
		{
			if (hasAutoIndex())
			{
				generateAutoIndex();
				_response.setStatusCode(OK);
				return; // listing HTML genere directement enregistre dans le body de response?
			}
			else
				_response.setStatusCode(FORBIDDEN); //Directory listing forbidden
			return;
		}
	}

	int	fd = openReadFile(_resolved_path);
	if (fd < 0)
		return;

	int target_size = fileSize(_resolved_path);

	/* Init Response */
	_response.setStatusCode(OK);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Content-Type", getContentType(_resolved_path));
	_response.setHeader("Content-Length", intToString(target_size));
	_response.setBodyFd(fd);
	_response.setBodySize(target_size);
	_response.setStatusCode(OK);
}

/* INDEX/DIRECTORY HANDLING */

bool	RequestHandler::resolveIndex()
{
	std::vector<std::string> indexes =_server.getIndexes();

	if (indexes.empty())
		return false;

	std::string	dir_path = _resolved_path;

	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string test_path = dir_path + indexes[i];
		if (access(test_path.c_str(), R_OK) == 0)
		{
			_resolved_path = test_path;
			_is_directory = false;
			std::cout << "[DEBUG] index found: " << _resolved_path << std::endl;
			return true;
		}
	}
	return false;
}

bool	RequestHandler::hasAutoIndex()
{
	if (_matched_location)
		return (_matched_location->getAutoIndex());
	return _server.getAutoindex();
}

void	RequestHandler::generateAutoIndex()
{
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
			_response.setStatusCode(FORBIDDEN);
			break;
		case ENOENT:
			_response.setStatusCode(NOT_FOUND);
			break;
		default:
			_response.setStatusCode(INTERNAL_SERVER_ERROR);
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
			_response.setStatusCode(FORBIDDEN);
			break;
		case ENOENT:
			_response.setStatusCode(NOT_FOUND);
			break;
		default:
			_response.setStatusCode(INTERNAL_SERVER_ERROR);
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

/* Utils */

std::string intToString(size_t value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}


/* TESTS/DEBUG */

void	RequestHandler::printRoutes()
{
	std::map<std::string, Location>::const_iterator it;

	std::cout << "---------Print routes---------"<< std::endl;
	for (it = _server.getLocations().begin(); it != _server.getLocations().end(); it++)
	{
		std::cout << "Key: " << it->first
		<< "\nName-> " << it->second.getName()
		<< "\nRoot-> " << it->second.getRoot()
		<< "\nAlias-> " << it->second.getAlias() << std::endl;
	}
	std::cout << "---------------------------"<< std::endl;

}
