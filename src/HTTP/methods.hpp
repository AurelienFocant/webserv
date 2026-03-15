#ifndef METHODS_HPP
#define METHODS_HPP

#include <string>
#include "pathResolver.hpp"

class Response;
class Request;

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

namespace upload
{
	bool			hasContentTypeHeader(const Request& req);
	bool			isMultiformData(const Request& req);
	std::string		extractBoundary(const Request& req);
	std::string		extractFilename(const Request& req, std::string boundary);
	std::string		verifyFile(const PathContext& ctx, std::string filename);
	bool			saveDataToFile(const Request& req, std::string filename, std::string boundary);
}

#endif
