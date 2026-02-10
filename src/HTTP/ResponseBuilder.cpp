#include "ResponseBuilder.hpp"
#include "RequestHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "autoindex.hpp"
#include "HtmlBuilder.hpp"
#include "cgi.hpp"
#include "handleUser.hpp"

/* ////////////REQUEST _handler////////////////// */

ResponseBuilder::ResponseBuilder(const Request& request, Response& response, const std::map<int, std::string>& error_pages)
: _request(request), _response(response), _error_pages(error_pages){}

//ResponseBuilder::ResponseBuilder(const ResponseBuilder& other) : _handler(other._handler) {}

ResponseBuilder::~ResponseBuilder() {}

/* ResponseBuilder&	ResponseBuilder::operator=(const ResponseBuilder& rhs)
{
	if (this != &rhs)
		_handler = rhs._handler;
	return *this;
} */


/* BUILD RESPONSE */

void	ResponseBuilder::setBaseResponse(int status_code)
{
	_response.setStatusCode(status_code);
	_response.setHttpVersion(_request.getHttpVersion());
	_response.setHeader("Date", httpUtils::getTime());
}

void	ResponseBuilder::buildFileResponse(int fd, const std::string& path)
{
	int target_size = fileSystem::fileSize(path);

	setBaseResponse(OK);
	_response.setHeader("Content-Length", httpUtils::intToString(target_size));
	_response.setHeader("Content-Type", fileSystem::getContentType(path));
	_response.setBodyFd(fd);
	_response.setBodySize(target_size);
}

void	ResponseBuilder::buildHtmlResponse(const std::string& content, const std::string& path)
{
	setBaseResponse(OK);
	_response.setHeader("Content-Length", httpUtils::intToString(content.size()));
	_response.setHeader("Content-Type", fileSystem::getContentType(path));
	_response.setBodyContent(content);
	_response.setBodySize(content.size());
}

void	ResponseBuilder::buildRedirectResponse(int status_code, const std::string& redirect_uri)
{
	setBaseResponse(status_code);
	_response.setHeader("Location", redirect_uri);
	_response.setHeader("Content-Length", "0");
	_response.setBodySize(0);
}


void	ResponseBuilder::buildErrorResponse(int status_code)
{
	int 	fd = -1;
	size_t	file_size;

	setBaseResponse(status_code);
	if (loadErrorPage(status_code, fd, file_size))
	{
		_response.setHeader("Content-Length", httpUtils::intToString(file_size));
		_response.setHeader("Content-Type", "text/html");
		_response.setBodyFd(fd);
		_response.setBodySize(file_size);
	}

}

void	ResponseBuilder::buildMethodAllowedResponse(int status_code, const std::set<std::string>& allowed)
{
	setBaseResponse(status_code);

	std::string allow_header;
	for (std::set<std::string>::const_iterator it = allowed.begin();
		it != allowed.end(); it++)
	{
		if (it != allowed.begin())
			allow_header += ", ";
		allow_header += *it;
	}

}

/* ERRORS */

bool	ResponseBuilder::loadErrorPage(int status_code, int& fd, size_t& size)
{

	std::map<int, std::string>::const_iterator it = _error_pages.find(status_code);
	if (it == _error_pages.end())
		return false;
	
	std::string error_path = it->second;
	// check if absolute path or string to concatene

	fd = fileSystem::openReadFile(error_path);
	if (fd < 0)
	{
		_response.setStatusCode(httpUtils::errnoToHttpStatus(errno));
		return false;
	}

	size = fileSystem::fileSize(error_path);
	
	return true;
}