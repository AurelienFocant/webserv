
#include "Connection.hpp"
#include "RequestHandler.hpp"
#include "ResponseBuilder.hpp"
#include "VirtualServer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "autoindex.hpp"
#include "HtmlBuilder.hpp"
#include "cgi.hpp"
#include "handleUser.hpp"

/* ////////////REQUEST HANDLER////////////////// */

RequestHandler::RequestHandler(Connection& currConn) 
	: _request(currConn.request)
	, _response(currConn.response)
	, _server(currConn.virtual_server)
	, _builder(currConn.request, currConn.response, _server.getErrorPages())
	, _root(currConn.virtual_server.getRoot())
	, _cage_root("")
	, _request_path("")
	, _resolved_path("")
	, _matched_location(NULL)
	, _matched_extension(NO_EXT)
	, _is_directory(false)
	, _script_name("")
	, _path_info("")
	, _query("")
	, _connection(currConn)
{}

RequestHandler::~RequestHandler() {}


void	RequestHandler::handleRequest()
{
	if (_request.getStatusCode() != OK)
	{
		_builder.buildErrorResponse(_request.getStatusCode());
		return;
	}

	if (!extractPath() || !resolvePath() || !processMethods())
	{
		if (!hasRedirect())
			_builder.buildErrorResponse(_response.getStatusCode());
		return;
	}
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
		if (detectCGI())
			_response.setBodyType(DYNAMIC);

		//Detect virtual location
		if (_matched_location->getVirtual())
			return true;

		//Config Redirections
		if (handleConfigRedirect())
			return false;

		if (!normalizePath())
			return false;
		
		if (!_matched_location->getAlias().empty())
		{
			//Alias : replace prefix of location
			std::string	location_path = _matched_location->getName();
			std::string	remaining_path = _request_path.substr(location_path.size());
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
		if (!normalizePath())
			return false;

		//Default root
		_resolved_path = _root + _request_path;
	}

	std::cout << "[DEBUG] Full Path: " << _resolved_path << std::endl;

	if (!validatePath())
		return false;

	return true;

}

bool	RequestHandler::handleConfigRedirect()
{
	std::string redirect_uri = _matched_location->getRedirect();
	int			redirect_code = _matched_location->getRedirectCode();

	if (redirect_uri.empty() || !redirect_code)
		return false;
	
	_builder.buildRedirectResponse(redirect_code, redirect_uri);

	return true;
}

bool	RequestHandler::handleTrailingSlash()
{
	if (_matched_location && _matched_location->getVirtual())
		return true;

	if (_request_path[_request_path.size() - 1] == '/')
		return false;

	std::string redirect_uri = _request_path + "/";
	if (!_query.empty())
		redirect_uri += '?' + _query;
	_builder.buildRedirectResponse(MOVED_PERMANENTLY, redirect_uri);

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
				if (ext_end == _request_path.size() || _request_path[ext_end - 1] == '/' || _request_path[ext_end] == '/')
				{
					//_matched_location = location;
					if (_matched_extension == NO_EXT)
						_matched_extension = extensionFromString(ext);
				}
			}
		}
		else if (_request_path.find(route_path, 0) == 0)
		{
			size_t route_len = route_path.size();

			// Test if it's a real match, not just a partial prefix
			if (_request_path.size() == route_len || _request_path[route_len - 1] == '/' || _request_path[route_len] == '/')
			{
				//Keep the longest match
				if (route_len > longest_match)
				{
					longest_match = route_path.size();
					_matched_location = location;
					_cage_root = location->getName();
				}
			}
		}
	}

	std::cout << "[DEBUG] Matched extension: " << extensionToString(_matched_extension) << std::endl;
	if (_matched_location)
		std::cout << "[DEBUG] Matched location: " << _matched_location->getName() << std::endl;
	else
		std::cout << "[DEBUG] No location matched: " << std::endl;
}

bool	RequestHandler::detectCGI()
{
/* 	if (!_matched_location->getCGI())
		return false; */
	
	if (_matched_extension == NO_EXT || _matched_extension == UNKNOWN_EXT)
		return false;

	if (_matched_extension == NO_EXT)
		return false;

	std::string ext_str = extensionToString(_matched_extension);

	size_t ext_start = _request_path.find(ext_str);
	if (ext_start == std::string::npos)
		return false;
	size_t ext_end = ext_start + ext_str.size();

	_script_name = _request_path.substr(0, ext_end);

	_path_info = "";
	if (ext_end < _request_path.size() && _request_path[ext_end] == '/')
	{
		_path_info = _request_path.substr(ext_end);
		_request_path = _request_path.substr(0, ext_end);
	}
	
	return true;

}

bool	RequestHandler::decodePath(const std::string& encoded, std::string& decoded)
{
	//avoid useless reallocations (borne superieure)
	decoded.reserve(encoded.size());

	for (size_t i = 0; i < encoded.size(); i++)
	{
		if (encoded[i] == '%' && i + 2 < encoded.size())
		{
			std::string	hex = encoded.substr(i + 1, 2);

			if (std::isxdigit(hex[0]) && std::isxdigit(hex[1]))
			{
				std::stringstream ss(hex);
				int	value;
				if (ss >> std::hex >> value)
				{
					if (value == 0)
						return false;
					decoded += static_cast<char>(value);
					i += 2;
				}
			}
			else
				decoded += encoded[i];
		}
		else
			decoded += encoded[i];
	}

	return true;
}

bool	RequestHandler::normalizePath()
{
	std::cout << "Cage root :" << _cage_root << std::endl;
	std::cout << "Request path :" << _request_path << std::endl;

	std::string	decoded_path;

	if (!decodePath(_request_path, decoded_path))
	{
		std::cerr << "[ERROR] Path traversal attempt" << std::endl;
		_response.setStatusCode(403);
		return false;
	}
	std::cout << "[DEBUG] Decoded path: " << decoded_path << std::endl;

	// fonctionne meme hors location car _cage_root est initialise a "";
	std::string temp_path = decoded_path.substr(_cage_root.size());

	if (!temp_path.empty() && temp_path[0] != '/')
		temp_path = "/" + temp_path;

	bool trailing_slash = (!temp_path.empty()
		&& temp_path[temp_path.size() - 1] == '/');

	if (!trailing_slash)
		temp_path += '/';

	std::cout << "[DEBUG] Path to normalize " << temp_path << std::endl;

	std::vector<std::string> segments;

	size_t pos = 0;
	size_t start = 0;

	while ((pos = temp_path.find('/', start)) != std::string::npos && pos < temp_path.size())
	{
		std::string seg = temp_path.substr(start, pos -start);

		if (seg.empty() || (seg.at(0) == '.' && seg.size() == 1))
		{
			start = pos + 1;
			continue;
		}
		else if (seg == ".." )
		{
			if (segments.empty())
			{
				std::cerr << "[ERROR] Path traversal attempt" << std::endl;
				_response.setStatusCode(403);
				return false;
			}
			segments.pop_back();
		}
		else
			segments.push_back(seg);
		start = pos + 1;
	}

	temp_path = _cage_root;

	for (size_t i = 0; i < segments.size(); i++)
		temp_path += "/" + segments[i];

	if (temp_path.empty() || (trailing_slash && temp_path.size() != 1))
		temp_path += '/';
	
	_request_path = temp_path;

	std::cout << "[DEBUG] Normalized path " << _request_path << std::endl;

	return true;
}


bool	RequestHandler::validatePath()
{
	struct stat statBuf;
	
	if (_matched_location && _matched_location->getVirtual())
		return true;

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

	if (_is_directory && handleTrailingSlash())
		return false;

	return true;
}

/* METHODS PROCESSING */

bool	RequestHandler::isAllowedMethod()
{
	if (!_matched_location)
		return true;

	std::set<std::string> allowed = _matched_location->getAllowedMethods();

	// if no allowd_methods directive in this location in config true by default?
	if (allowed.empty())
		return true;

	std::string method = methodToString(_request.getMethod());

	if (allowed.find(method) == allowed.end())
	{
		_builder.buildMethodAllowedResponse(METHOD_NOT_ALLOWED, allowed);
		return false;
	}
	return true;
}

bool	RequestHandler::processMethods()
{
	if (!isAllowedMethod())
		return false;
	if (_response.getBodyType() == DYNAMIC)
		return (true);

	switch(_request.getMethod())
	{
		case GET:
			return processGetMethod();
 		case POST:
			return processPostMethod();
/*
			case DELETE:
			return processDeleteMethod();
*/
		default: 
			_response.setStatusCode(METHOD_NOT_ALLOWED); // ? 
			return false;
	}
	return (true);
}

bool	RequestHandler::processGetMethod()
{
	if (_matched_location && _matched_location->getVirtual())
	{
		//do some action
		return true;
	}

	//STATIC
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

	int	fd = fileSystem::openReadFile(_resolved_path);
	if (fd < 0)
	{
		_response.setStatusCode(httpUtils::errnoToHttpStatus(errno));
		return false;
	}

	_builder.buildFileResponse(fd, _resolved_path);

	return true;
}


bool	RequestHandler::processPostMethod() {
	if (_matched_location->getName() == "createuser") {
		_response.setStatusCode(handleUser::createNewUser(*this));
	}
	if (_matched_location->getName() == "comment") {
	//	use script create comment;
	}
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
	if (!dir_path.empty() && dir_path[dir_path.size() -1] != '/')
		dir_path += '/';

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

	_builder.buildHtmlResponse(html, _resolved_path);
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

/*Getters*/
const Request&	RequestHandler::getRequest() const {
	return (_request);
}

const Response&	RequestHandler::getResponse() const {
	return (_response);
}
