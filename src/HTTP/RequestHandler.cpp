
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

RequestHandler::RequestHandler() 
	: _server(NULL)
	, _matched_location(NULL)
	, _matched_extension(NO_EXT)
	, _is_directory(false)
	, _request()
	, _response()
{}

RequestHandler::~RequestHandler() {}

void	RequestHandler::processRequest(const std::string& input)
{
		_request.addInput(input);
		_request.parseRequest();
		return ;
}

void	RequestHandler::processBody()
{
	if (_matched_location)
		_request.handleBody((*_matched_location).getMaxBodySize());
	else
		_request.handleBody((*_server).getMaxBodySize());
	return ;
}

void	RequestHandler::addInput(const std::string& input)
{
	_request.addInput(input);
}


void	RequestHandler::handleRequest()
{
	if (_request.getStatusCode() != OK)
		_response.setStatusCode(_request.getStatusCode());
	else
		resolvePath() && processMethods();
	if (_response.isCGI && _response.getStatusCode() < 300)
		return;
	resp::prepareResponse(_response, _request, _server->getErrorPages());
}


/* METHODS PROCESSING */


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

	int bytes_sent = 0;
	int start = 0;
	while ((bytes_sent = write(fd, &(s_buffer.c_str()[start]), 4000)) > 0) {
		start += bytes_sent;
	}
	close(fd);
	return (true);
}

void	RequestHandler::requestIsComplete() {
	_request.setComplete(true);
	return ;
}


/* Getters */
const Request&	RequestHandler::getRequest() const
{
	return (_request);
}

const Response&	RequestHandler::getResponse() const {
	return (_response);
}

const VirtualServer*	RequestHandler::getVirtualServer() const
{
	return _server;
}

/* Setters */
void	RequestHandler::setVirtualServer(const VirtualServer& server)
{
	_server = &server;
}

void	RequestHandler::setRoot(const std::string& root)
{
	_root = root;
}

void	RequestHandler::clean()
{
	_request.cleanRequest();
	_response.cleanResponse();
	_server = NULL;
	_root.clear();
	_request_path.clear();
	_resolved_path.clear();
	_cgi_exec.clear();
	_matched_location = NULL;
	_matched_extension = NO_EXT;
	_ext_str.clear();
	_is_directory = false;
	_script_name.clear();
	_path_info.clear();
	_query.clear();
}
