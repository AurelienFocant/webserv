
#include "Connection.hpp"
#include "RequestHandler.hpp"
#include "VirtualServer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "autoindex.hpp"
#include "HtmlBuilder.hpp"
#include "cgi.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

/* ////////////REQUEST HANDLER////////////////// */

RequestHandler::RequestHandler(Connection& currConn) 
	: _request(currConn.request)
	, _response(currConn.response)
	, _server(currConn.virtual_server)
	, _root(currConn.virtual_server.getRoot())
	, _request_path("")
	, _resolved_path("")
	, _matched_location(NULL)
	, _matched_extension(NO_EXT)
	, _is_directory(false)
	, _script_name("")
	, _path_info("")
	, _query("")
{}

RequestHandler::~RequestHandler() {}


void	RequestHandler::handleRequest()
{
	if (_request.getStatusCode() != OK)
		_response.setStatusCode(_request.getStatusCode());
	else
		extractPath() && resolvePath() && processMethods();
	if (_response.isCGI && _response.getStatusCode() < 300)
		return;
	resp::prepareResponse(_response, _request, _server.getErrorPages());
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
		{
			if (!validateCGIScript())
				return false;
			_response.isCGI = true; //something like that
			return true;
		}

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
			std::string	location_path	= _matched_location->getName();
			std::string	remaining_path	= _request_path.substr(location_path.size());
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

	// std::cout << "[DEBUG] Full Path: " << _resolved_path << std::endl;

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
	
	_response.setHeader("Location", redirect_uri);
	_response.setStatusCode(redirect_code);

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

	_response.setHeader("Location", redirect_uri);
	_response.setStatusCode(MOVED_PERMANENTLY);

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
			size_t start = _request_path.find(ext);
			if (start != std::string::npos)
			{
				size_t ext_end = start + ext.size();
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
				}
			}
		}
	}

	// std::cout << "[DEBUG] Matched extension: " << extensionToString(_matched_extension) << std::endl;
	// if (_matched_location)
	// 	std::cout << "[DEBUG] Matched location: " << _matched_location->getName() << std::endl;
	// else
	// 	std::cout << "[DEBUG] No location matched: " << std::endl;
}

bool	RequestHandler::detectCGI()
{
	if (!_matched_location->getCGI())
		return false;
	
	if (_matched_extension == NO_EXT || _matched_extension == UNKNOWN_EXT)
		return false;

	std::string ext_str = extensionToString(_matched_extension);

	size_t start = _request_path.find(ext_str);
	if (start == std::string::npos)
		return false;
	size_t ext_end = start + ext_str.size();

	_script_name = _request_path.substr(0, ext_end);

	_path_info = "";
	if (ext_end < _request_path.size() && _request_path[ext_end] == '/')
	{
		_path_info = _request_path.substr(ext_end);
		_request_path = _request_path.substr(0, ext_end);
	}
	return true;
}

bool	RequestHandler::validateCGIScript()
{
	std::string cgi_bin_path = 	_root + "/cgi-bin/";
	DIR* dir_ptr = opendir(cgi_bin_path.c_str());
	if (!dir_ptr)
	{
		// throw exception that set status to INTERNAL_ERROR?
		_response.setStatusCode(INTERNAL_SERVER_ERROR);
		return false;
	}
	size_t start = _script_name.rfind('/');
	if (start == std::string::npos)
	{
		closedir(dir_ptr);
		return false;
	}

	std::string script_basename = _script_name.substr(start + 1);
	struct dirent* dir_entry;
	while ((dir_entry = readdir(dir_ptr)) != NULL)
	{
		if ((std::strcmp(dir_entry->d_name, script_basename.c_str()) == 0))
		{
			_resolved_path = _root + "/cgi-bin/" + script_basename;
			closedir(dir_ptr);
			return true;
		}
	}
	closedir(dir_ptr);
	_response.setStatusCode(FORBIDDEN);
	return false;
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
	std::string cage_root = !_matched_location ? "" : _matched_location->getName();

	std::cout << "Cage root :" << cage_root << std::endl;
	std::cout << "Request path :" << _request_path << std::endl;

	std::string	decoded_path;

	if (!decodePath(_request_path, decoded_path))
	{
		std::cerr << "[ERROR] Path traversal attempt" << std::endl;
		_response.setStatusCode(403);
		return false;
	}
	// std::cout << "[DEBUG] Decoded path: " << decoded_path << std::endl;

	// fonctionne meme hors location car cage_root est initialise a "";
	std::string temp_path = decoded_path.substr(cage_root.size());

	bool slash[2] = {true, true};

	if (!temp_path.empty() && temp_path[0] != '/')
	{
		slash[0] = false;
		temp_path = "/" + temp_path;
	}
	slash[1] = (!temp_path.empty()
		&& temp_path[temp_path.size() - 1] == '/');
	if (!slash[1])
		temp_path += '/';

	// std::cout << "[DEBUG] Path to normalize " << temp_path << std::endl;

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

	temp_path = cage_root;

	for (size_t i = 0; i < segments.size(); i++)
	{
		if (!slash[0] && i == 0)
			temp_path += segments[i];
		else
			temp_path += "/" + segments[i];
	}

	if (temp_path.empty() || (slash[1] && temp_path.size() != 1))
		temp_path += '/';
	
	_request_path = temp_path;

	// std::cout << "[DEBUG] Normalized path " << _request_path << std::endl;

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
	// ! autorisation par default?
	if (!_matched_location)
		return true;

	std::set<std::string> allowed = _matched_location->getAllowedMethods();

	// if no allowd_methods directive in this location in config true by default?
	if (allowed.empty())
		return true;

	std::string method = methodToString(_request.getMethod());

	if (allowed.find(method) == allowed.end())
	{
		_response.setStatusCode(METHOD_NOT_ALLOWED);
		_response.setHeader("Allow", resp::buildAllowHeader(allowed));
		return false;
	}
	return true;
}

bool	RequestHandler::processMethods()
{
	if (!isAllowedMethod())
		return false;
	if (_response.isCGI)
		return (true);

	switch(_request.getMethod())
	{
		case GET:
			return processGetMethod();
 		case POST:
			return processPostMethod();
		case DELETE:
			return processDeleteMethod();
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

	// ADD CGI

/* 	int	fd = fileSystem::openReadFile(_resolved_path);
	if (fd < 0)
	{
		_response.setStatusCode(httpUtils::errnoToHttpStatus(errno));
		return false;
	} */

	if (!resp::loadBody(_response, _resolved_path))
		return false;

	return true;
}

bool	RequestHandler::_hasContentTypeHeader(void)
{
	if (_request.getHeaderValues("Content-Type").size()) {
		if (!_request.getHeaderValues("Content-Type")[0].empty())
			return (true);
	}
	return (false);
}

bool	RequestHandler::_isMultiformData(void)
{
	size_t	pos = std::string::npos;

	std::string	header = _request.getHeaderValues("Content-Type")[0];
	if ((pos = header.find("multipart/form-data")) != std::string::npos) {
		return (true);
	}
	return (false);
}

std::string	RequestHandler::_extractBoundary(void)
{
	size_t	pos = std::string::npos;
	std::string boundary_str(" boundary=");

	std::string	header = _request.getHeaderValues("Content-Type")[0];
	if ((pos = header.find(boundary_str)) != std::string::npos) {
		std::string res = header.substr(pos + boundary_str.size());
		return (res);
	}
	return ("");
}

std::string	RequestHandler::_extractFilename(std::string boundary)
{
	size_t start_of_filename;
	size_t end_of_filename;
	std::string body = _request.getBody();

	if (!body.find(boundary))
		return "";

	std::string file("filename=");
	start_of_filename = body.find(file);
	if (start_of_filename == std::string::npos)
		return "";

	start_of_filename += file.size();
	end_of_filename = body.find("\r\n", start_of_filename);
	if (end_of_filename == std::string::npos)
		return "";

	if (body[start_of_filename] == '"') {
		if (body[end_of_filename - 1] != '"')
			return "";
		start_of_filename++;
		end_of_filename--;
	}

	std::string filename = body.substr(start_of_filename, end_of_filename - start_of_filename);
	return (filename);
}

std::string	RequestHandler::_verifyFile(std::string filename)
{
	std::string dir = getResolvedPath();
	if (!fileSystem::isDir(dir))
		return ("");

	if (dir[dir.size() - 1] != '/')
		dir = dir + "/";

	if (!fileSystem::isExecutable(dir) || !fileSystem::isWritable(dir))
		return ("");

	filename = dir + filename;
	if (!fileSystem::isFile(filename))
		return (filename);

	if (!fileSystem::isReadable(filename) || !fileSystem::isWritable(filename))
		return ("");

	if (filename.find("../") != std::string::npos)
		return ("");

	return (filename);
}

bool	RequestHandler::_saveDataToFile(std::string filename)
{
	std::string body = _request.getBody();

	size_t end_of_header = body.find("\r\n\r\n");
	if (end_of_header == std::string::npos)
		return (false);
	end_of_header += 4;

	int fd = open(filename.c_str(), O_WRONLY | O_CREAT, 0664);
	if (fd < 0)
		return (false);

	std::string s_buffer = body.substr(end_of_header);
	int bytes_to_send = s_buffer.size();

	int bytes_sent = 0;
	int start = 0;
	while ((bytes_sent = write(fd, &(s_buffer.c_str()[start]), 4000)) > 0) {
		bytes_to_send -= bytes_sent;
		start += bytes_sent;
		std::cout << bytes_to_send << " left to send" << std::endl;
	}
	close(fd);
	return (true);
}

bool	RequestHandler::processPostMethod()
{
	if (!_hasContentTypeHeader()) {
		_response.setStatusCode(BAD_REQUEST);	return (false);
	}

	if (!_isMultiformData()) {
		_response.setStatusCode(BAD_REQUEST);	return (false);
	}

	std::string boundary = _extractBoundary();
	if (boundary.empty()) {
		_response.setStatusCode(BAD_REQUEST);	return (false);
	}

	std::string filename = _extractFilename(boundary);
	if (filename.empty()) {
		_response.setStatusCode(BAD_REQUEST);	return (false);
	}

	filename = _verifyFile(filename);
	if (filename.empty()) {
		_response.setStatusCode(FORBIDDEN);		return (false);
	}

	if (!_saveDataToFile(filename)) {
		_response.setStatusCode(INTERNAL_SERVER_ERROR);	return (false);
	};

	_response.setStatusCode(CREATED);
	_response.setHeader("Content-Length", "0");
	_response.setHttpVersion(_request.getHttpVersion());
	return (true);
}

bool	RequestHandler::processDeleteMethod()
{
	if (_is_directory) {
		_response.setStatusCode(FORBIDDEN); return (false);
	}

	if (!fileSystem::isFile(_resolved_path)) {
		_response.setStatusCode(NOT_FOUND); return (false);
	}

	std::string dirname	= fileSystem::getDirname(_resolved_path);
	if (!fileSystem::isWritable(dirname) || !fileSystem::isExecutable(dirname)) {
		_response.setStatusCode(FORBIDDEN); return (false);
	}


	if (unlink(_resolved_path.c_str()) != 0) {
		_response.setStatusCode(FORBIDDEN); return (false);
	}


	_response.setStatusCode(NO_CONTENT);
	return (true);
}

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

	_response.setBody(html);
	_response.setHeader("Content-Type", "text/html");
}

/*Getters*/
const Request&	RequestHandler::getRequest() const
{
	return (_request);
}

const Response&	RequestHandler::getResponse() const {
	return (_response);
}
