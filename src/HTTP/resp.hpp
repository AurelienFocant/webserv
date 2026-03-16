#ifndef RESP_HPP
#define RESP_HPP

#include <string>
#include <map>
#include <set>

class Response;
class Request;
class RequestHandler;

namespace resp
{
	void			prepareResponse(RequestHandler & reqHandler);
	bool 			loadBody(Response& response, const std::string& path);
	bool 			loadFileToString(const std::string& path, std::string& result);
	bool			loadErrorPage(RequestHandler &reqHandler, int status_code, const std::map<int, std::string>& error_pages, std::string& result);
	std::string		buildAllowHeader(const std::set<std::string>& allowed_methods);
}

#endif
