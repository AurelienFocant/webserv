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
	void		matchLocation(PathContext& ctx, const VirtualServer* server);
	bool		resolve(PathContext& ctx, Response& resp);
	bool		validate(PathContext& ctx, Response& resp);

	bool		decodePath(const std::string& encoded, std::string& decoded);
	bool		normalizePath(PathContext& ctx, Response& resp);
	bool		detectCGI(PathContext& ctx);
	bool		handleConfigRedirect(PathContext& ctx, Response& resp);
	bool		hasTrailingSlash(PathContext& ctx, Response& resp);
	bool		hasRedirect(const Response& resp);
}

namespace method
{
	bool		dispatch(PathContext& ctx, Request& req, Response& resp);
	bool		isAllowed(const PathContext& ctx, Request& req, Response& resp);

	bool		processGet(PathContext& ctx, Response& resp);
	bool		processHead(PathContext& ctx, Response& resp);
	bool		processPost(PathContext& ctx, const Request& req, Response& resp);
	bool		processDelete(const PathContext& ctx, Response& resp);

	bool		resolveIndex(PathContext& ctx);
	bool		hasAutoIndex(const PathContext& ctx);
	void		generateAutoIndex(const PathContext& ctx, Response& resp);
}

#endif