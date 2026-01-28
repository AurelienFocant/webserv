#include "RequestHandler.hpp"
#include "autoindex.hpp"
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
 
	if (_matched_location)
	{
		//Config Redirections
		if (handleConfigRedirect())
			return false;
		else if (!_matched_location->getAlias().empty())
		{
			//Alias : replace prefix of location
			std::string	location_path = _matched_location->getName();
			std::string	remaining_path = _request_path.substr(location_path.length());
			_resolved_path = _matched_location->getAlias() + remaining_path;
		}
		else if (!_matched_location->getRoot().empty())
		{
			//Location root : replace main root
			_root = _matched_location->getRoot();
			_resolved_path = _root + _request_path;
		}
		else
		{
			//Default root
			_resolved_path = _root + _request_path;
		}
	}
	else
	{
		//Default root
		_resolved_path = _root + _request_path;
	}

	std::cout << "[DEBUG] Full Path: " << _resolved_path << std::endl;

	if (!validatePath() || handleTrailingSlash())
		return false;

	return true;

}

bool	RequestHandler::handleConfigRedirect()
{
	if (_matched_location->getRedirect().empty() || !_matched_location->getRedirectCode())
		return false;
	
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setStatusCode(_matched_location->getRedirectCode());
	_response.setHeader("Location", _matched_location->getRedirect());
	_response.setHeader("Content-Length", "0");

	return true;
}

bool	RequestHandler::handleTrailingSlash()
{
	if (_request_path[_request_path.length() - 1] == '/')
		return false;

	std::string redirect_url = _request_path + "/";
	//if query??

	_response.setHttpVersion(_request.getHttpVersion());
	_response.setStatusCode(MOVED_PERMANENTLY);
	_response.setHeader("Location", redirect_url);
	_response.setHeader("Content-Length", "0");

	return true;
}

bool	RequestHandler::hasRedirect()
{
	int status = _response.getStatusCode();

	return status == MOVED_PERMANENTLY
		|| status == FOUND
		|| status == SEE_OTHER
		|| status == TEMPORARY_REDIRECT;
}


void	RequestHandler::findLocation()
{
	size_t		longest_match = 0;

	for (std::map<std::string, Location>::const_iterator it = _server.getLocations().begin(); it != _server.getLocations().end(); it++)
	{
		const std::string&	route_path = it->first;
		const Location*		location = &(it->second);

		//Check if the requested path begin with a Location name
		if (_request_path.find(route_path, 0) == 0)
		{
			size_t route_len = route_path.length();

			// Test if it's a real match, not just a partial prefix
			if (_request_path.length() == route_len || _request_path[route_len] == '/')
			{
				//Keep the longest match
				if (route_len > longest_match)
				{
					longest_match = route_path.length();
					_matched_location = location;
				}
			}
		}
	}

	if (_matched_location)
		std::cout << "[DEBUG] Matched location: " << _matched_location->getName() << std::endl;
	else
		std::cout << "[DEBUG] No location matched: " << std::endl;
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
	if (hasRedirect())
	{
		std::cout << "[DEBUG] Redirect code: " << _response.getStatusCode()
		<< " skip processMethods() " << std::endl;
		return false;
	}

	switch(_request.getMethod())
	{
		case GET:
			return processGetMethod();
/* 		case POST:
			return processPostMethod();
		case DELETE:
			return processDeleteMethod(); */
		default: 
			_response.setStatusCode(METHOD_NOT_ALLOWED); // ? 
			return false;
	}
}

bool	RequestHandler::processGetMethod()
{
	if (_is_directory)
	{
		if (!resolveIndex())
		{
			if (hasAutoIndex())
			{
				generateAutoIndex();
				_response.setStatusCode(OK);
				return true;
			}
			else
				_response.setStatusCode(FORBIDDEN); //Directory listing forbidden
			return false;
		}
	}

	int	fd = openReadFile(_resolved_path);
	if (fd < 0)
		return false;

	int target_size = fileSize(_resolved_path);

	/* Init Response */
	_response.setStatusCode(OK);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Content-Type", getContentType(_resolved_path));
	_response.setHeader("Content-Length", intToString(target_size));
	_response.setBodyFd(fd);
	_response.setBodySize(target_size);

	return true;
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
/* 	if (_matched_location)
		return (_matched_location->getAutoIndex());
	return _server.getAutoindex(); */
	return false;
}

void	RequestHandler::generateAutoIndex()
{
	std::string	html = ::generateAutoIndex(_resolved_path);

	_response.setStatusCode(OK);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Content-Type", "text/html");
	_response.setHeader("Content-Length", intToString(html.size()));
	_response.setBodyContent(html);
	_response.setBodySize(html.size());

	std::cout << "[DEBUG] Autoindex size: " << html.size() << " bytes" << std::endl;
	std::cout << "[DEBUG] Html generated: " << html << std::endl;

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
        return "application/octet-stream";
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
