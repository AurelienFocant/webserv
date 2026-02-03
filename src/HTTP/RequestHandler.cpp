#include "RequestHandler.hpp"
#include "Response.hpp"
#include "autoindex.hpp"
#include "HtmlBuilder.hpp"
#include "cgi.hpp"

/* ////////////REQUEST HANDLER////////////////// */

RequestHandler::RequestHandler(Connection& currConn) 
	: _request(currConn.request)
	, _response(currConn.response)
	, _server(currConn.virtual_server)
	, _root(currConn.virtual_server.getRoot())
	, _request_path("")
	, _resolved_path("")
	, _query("")
	, _matched_location(NULL)
	, _matched_extension(NO_EXT)
	, _is_directory(false)
{
	_response.setStatusCode(_request.getStatusCode());
	//printRoutes();
}

RequestHandler::~RequestHandler() {}


void	RequestHandler::handleRequest()
{
	if (_request.getStatusCode() != OK)
	{
		buildErrorResponse(_request.getStatusCode());
		return;
	}

	if (!extractPath() || !resolvePath() || !processMethods())
	{
		if (!hasRedirect())
			buildErrorResponse(_response.getStatusCode());
		return;
	}
}

/* PATH PROCESSING */

bool	RequestHandler::extractPath()
{
	if (_request.getRequestUri().empty() /* || _request.getRequestUri().at(0) != '/' */)
	{
		_response.setStatusCode(BAD_REQUEST);
		return false;
	}

	_request_path = _request.getRequestUri();
	//std::cout << "[DEBUG] RequestUri: " << _request.getRequestUri() << std::endl;
	//std::cout << "[DEBUG] Path " << _request_path << std::endl;

	size_t query_pos = _request_path.find("?");
	if (query_pos != std::string::npos)
	{
		_query = _request_path.substr(query_pos + 1);
		_request_path = _request_path.substr(0, query_pos);
	}
	
	return true;
}

bool	RequestHandler::resolvePath()
{
	findLocation();
 
	if (_matched_location)
	{
		//Detect CGI
		if (detectCgi())
			_response.setBodyType(DYNAMIC);

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

	if (!validatePath())
		return false;

	if (_is_directory && handleTrailingSlash())
		return false;

	//exit(2);

	return true;

}

bool	RequestHandler::handleConfigRedirect()
{
	std::string redirect_uri = _matched_location->getRedirect();
	int			redirect_code = _matched_location->getRedirectCode();

	if (redirect_uri.empty() || !redirect_code)
		return false;
	
	buildRedirectResponse(redirect_code, redirect_uri);

	return true;
}

bool	RequestHandler::handleTrailingSlash()
{
	if (_request_path[_request_path.length() - 1] == '/')
		return false;

	std::string redirect_uri = _request_path + "/";
	//if query??

	buildRedirectResponse(MOVED_PERMANENTLY, redirect_uri);

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
	std::string	ext;
	size_t		longest_match = 0;

	for (std::map<std::string, Location>::const_iterator it = _server.getLocations().begin(); it != _server.getLocations().end(); it++)
	{
		const std::string&	route_path = it->first;
		const Location*		location = &(it->second);


		//Check if the requested path match an extension or begin with a Location name
		if (route_path[route_path.size() -1] == '$')
		{
			ext = route_path.substr(0, route_path.size() -1);
			size_t ext_start = _request_path.find(ext);
			if (ext_start != std::string::npos)
			{
				size_t ext_end = ext_start + ext.size();
				if (ext_end == _request_path.size() || _request_path[ext_end] == '/')
				{
					_matched_location = location;
					_matched_extension = extensionFromString(ext);
					break ;
				}
			}
		}
		else if (_request_path.find(route_path, 0) == 0)
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

bool	RequestHandler::detectCgi()
{
/* 	if (!_matched_location->getCGI())
		return false; */
	
	if (_matched_extension == UNKNOWN_EXT)
		return false;

	std::string ext_str = extensionToString(_matched_extension);

	size_t ext_start = _request_path.find(ext_str);
	if (ext_start == std::string::npos)
		return false;
	size_t ext_end = ext_start + ext_str.size();

	std::string script_name = _request_path.substr(0, ext_end);
/* 	size_t	slash_pos = script_name.find_last_of('/');
	if (slash_pos != std::string::npos)
		script_name = script_name.substr(slash_pos + 1) */;

	std::string path_info = "";
	if (ext_end < _request_path.size() && _request_path[ext_end] == '/')
	{
		path_info = _request_path.substr(ext_end);
		_request_path = _request_path.substr(0, ext_end);
	}

	std::cout << "[DEBUG]: "
			<< "\nSCRIPT_NAME: " << script_name
			<< "\nEXT: " << ext_str
			<< "\nQUERY: " << _query
			<< "\nPATH_INFO: " << path_info << std::endl;
	
	return true;

}

bool	RequestHandler::normalizePath()
{
	// TO DO 

	// percent encoding?

	// "./" delete


	// "../" level up


	// "//" become "/"

	return true;
}


bool	RequestHandler::validatePath()
{
	struct stat statBuf;

	if (_resolved_path.find("/../") != std::string::npos)
	{
		_response.setStatusCode(403);
        std::cerr << "[DEBUG] Potential Traversal Path: "
		<< _resolved_path << std::endl;
		return false;
	}

	// normalizePath()


	if (stat(_resolved_path.c_str(), &statBuf) != 0)
	{
		if (errno == ENOENT)
			 _response.setStatusCode(NOT_FOUND);
		else if (errno == EACCES)
			_response.setStatusCode(FORBIDDEN);
		else
			_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return false;
/* 		std::cerr << "[DEBUG] stat() failed: " << strerror(errno) << std::endl;
        std::cerr << "[DEBUG] Current working dir: ";
        system("pwd") */;
	}

	_is_directory = S_ISDIR(statBuf.st_mode);

	return true;
}

/* METHODS PROCESSING */

bool	RequestHandler::processMethods()
{
	// Verifier AUTHORIZED METHODS in locations

	switch(_request.getMethod())
	{
		case GET:
			processGetMethod();
			break ;
/*
 		case POST:
			processPostMethod();
			break ;
*/
/*
			case DELETE:
			processDeleteMethod();
			break ;
*/
		default: 
			_response.setStatusCode(METHOD_NOT_ALLOWED); // ? 
			return false;
	}
	return (true);
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

	buildFileResponse(fd);

	return true;
}

bool	RequestHandler::processPostMethod() {
//	if (_matched_location->getCGI()) {
//		char **env = cgi::buildCgiEnv(_request);
//		cgi::execute(*this, _response, env);
//	}
//	if (_matched_location == "createuser")
//		utils::createUser();
//	if (_matched_location == "comment")
	//	use script create comment;
//	if (_matched_location == "createuser")
	return (true);		
}

/*
void	Request::processDelMethod() {
	if (_matched_location == "comment")
	//	use script delete comment;
}
*/

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
	std::string	html = ::generateAutoIndex(_resolved_path);

	buildHtmlResponse(html);
}

/* BUILD RESPONSE */

void	RequestHandler::buildFileResponse(int fd)
{
	int target_size = fileSize(_resolved_path);

	_response.setStatusCode(OK);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Content-Type", getContentType(_resolved_path));
	_response.setHeader("Content-Length", intToString(target_size));
	_response.setBodyFd(fd);
	_response.setBodySize(target_size);
}

void	RequestHandler::buildHtmlResponse(const std::string& content)
{
	_response.setStatusCode(OK);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Content-Type", "text/html");
	_response.setHeader("Content-Length", intToString(content.size()));
	_response.setBodyContent(content);
	_response.setBodySize(content.size());
}

void	RequestHandler::buildRedirectResponse(int status_code, const std::string& redirect_uri)
{
	_response.setStatusCode(status_code);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Location", redirect_uri);
	_response.setHeader("Content-Length", "0");
}

void	RequestHandler::buildErrorResponse(int status_code)
{
	int 	fd = -1;
	size_t	file_size;

	if (loadErrorPage(404, fd, file_size))
	{
		_response.setStatusCode(status_code);
		_response.setHttpVersion(_request.getHttpVersion());
		_response.setHeader("Content-Type", "text/html");
		_response.setHeader("Content-Length", intToString(file_size));
		_response.setBodyFd(fd);
		_response.setBodySize(file_size);
	}
/* 	else
	{
		std::string	error_content = generateDefaultError(status_code);

		_response.setStatusCode(status_code);
		_response.setHttpVersion(_request.getHttpVersion());
		_response.setHeader("Content-Type", "text/html");
		_response.setHeader("Content-Length", intToString(error_content.size()));
		_response.setBodyContent(error_content);
		_response.setBodySize(error_content.size());
	} */

}

/* ERRORS */

bool	RequestHandler::loadErrorPage(int status_code, int& fd, size_t& size)
{

	std::map<int, std::string>::const_iterator it = _server.error_pages.find(status_code);
	if (it == _server.error_pages.end())
		return false;
	
	std::string error_path = it->second;

	fd = openReadFile(error_path);
	if (fd < 0)
		return false;

	size = fileSize(error_path);
	
	return true;
}

std::string	RequestHandler::generateDefaultError(int status_code)
{
	std::stringstream	html;
	std::string			status_message = httpStatusToString(static_cast<t_HttpCode> (status_code));

	html << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head><title>Error " << status_code << "</title></head>\n"
		<< "<body style=\"font-family:Arial;text-align:center;padding-top:100px;\">\n"
		<< "    <h1>" << status_code << "</h1>\n"
		<< "    <p>" << status_message << "</p>\n"
		<< "</body>\n"
		<< "</html>";

	return html.str();
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
