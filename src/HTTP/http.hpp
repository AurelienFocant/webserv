#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include "PathContext.hpp"
#include "HTTPenum.hpp"

class Response;
class Request;
class VirtualServer;

namespace path
{
	bool		extract(PathContext& ctx, const Request& req, Response& resp);
	bool		matchLocation(PathContext& ctx, const VirtualServer* server);
	bool		resolve(PathContext& ctx, Response& resp);
	bool		validate(PathContext& ctx, Response& resp);

	bool		decodePath(const std::string& encoded, std::string& decoded, Response& resp);
	bool		normalizePath(PathContext& ctx, Response& resp);
	bool		detectCGI(PathContext& ctx);
	bool		handleConfigRedirect(PathContext& ctx, Response& resp);
	bool		hasTrailingSlash(PathContext& ctx, Response& resp);
	bool		hasRedirect(const Response& resp);
}

namespace method
{
	bool		dispatch(const PathContext& ctx, const Request& req, const Response& resp, const VirtualServer& server);
	bool		isAllowed(const PathContext& ctx, const Request& req, const Response& resp);

	bool		processGet(const PathContext& ctx, const Request& req, const Response& resp, const VirtualServer& server);
	bool		processHead(const PathContext& ctx, const Request& req, const Response& resp, const VirtualServer& server);
	bool		processPost(const PathContext& ctx, const Request& req, const Response& resp);
	bool		processDelete(const PathContext& ctx, const Request& req, const Response& resp);


	bool		resolveIndex(const PathContext& ctx, const VirtualServer& server);
	bool		hasAutoIndex(const PathContext& ctx, const VirtualServer& server);
	void		generateAutoIndex(const PathContext& ctx, Response& resp);
}

#endif