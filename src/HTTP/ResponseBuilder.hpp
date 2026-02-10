#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include <iostream>
#include <set>
#include <map>


// #include "RequestHandler.hpp"
// #include "Request.hpp"
// #include "Response.hpp"
#include "../Utils/fileSystem.hpp"
#include "../Utils/httpUtils.hpp"
#include "HTTPenum.hpp"

class Response;
class Request;

class ResponseBuilder
{
	private:

	/* Private Attributes */
	const Request&						_request;
	Response&							_response;
	const std::map<int, std::string>&	_error_pages;

	/* Private Methods */

	public:

	/* Constructors / Destructors */
	ResponseBuilder	(const Request& request, Response& response, const std::map<int, std::string>& error_pages);
	//ResponseBuilder	(const ResponseBuilder& other);
	~ResponseBuilder	();

	//ResponseBuilder&	operator=(const ResponseBuilder& rhs);

	/* Public Methods */
	void			setBaseResponse(int status_code);
	bool			loadErrorPage(int status_code, int& fd, size_t& size);
	void			buildFileResponse(int fd, const std::string& path);
	void			buildHtmlResponse(const std::string& content, const std::string& path);
	void			buildRedirectResponse(int status_code, const std::string& redirect_uri);
	void			buildErrorResponse(int status_code);
	void			buildMethodAllowedResponse(int status_code, const std::set<std::string>& allowed);

};

#endif
