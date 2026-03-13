
/* #include "paths.hpp"
#include "method.hpp" */
#include "http.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "VirtualServer.hpp"
#include "PathContext.hpp"
#include "../Utils/fileSystem.hpp"
#include "../Utils/httpUtils.hpp"

#include "Location.hpp"
#include "resp.hpp"
#include "upload.hpp"
#include "autoindex.hpp"
#include <unistd.h>

#include <sstream>
#include <sys/stat.h>
#include <stdio.h>
#include <cerrno>

/* PATH */
namespace path
{
	bool	extract(PathContext& ctx, const Request& req, Response& resp)
	{
		if (req.getRequestUri().empty() || req.getRequestUri().at(0) != '/')
		{
			resp.setStatusCode(BAD_REQUEST);
			return false;
		}

		ctx.request_path = req.getRequestUri();

		size_t query_pos = ctx.request_path.find("?");
		if (query_pos != std::string::npos)
		{
			ctx.query = ctx.request_path.substr(query_pos + 1);
			ctx.request_path = ctx.request_path.substr(0, query_pos);
		}
		
		return true;
	}

	void	matchLocation(PathContext& ctx, const VirtualServer* server)
	{
		std::string		ext;
		size_t			longest_match = 0;

/* 		// now in findLocation
		if (ctx.request_path.empty())
			extract(ctx, req, resp); */

		for (std::map<std::string, Location>::const_iterator it = server->getLocations().begin(); it != server->getLocations().end(); it++)
		{
			const std::string&	route_path = it->first;
			const Location*		location = &(it->second);

			//Check if the requested path match an extension or begin with a Location name
			if (route_path[route_path.size() -1] == '$')
			{
				ext = route_path.substr(0, route_path.size() -1);
				size_t start = ctx.request_path.find(ext);
				if (start != std::string::npos)
				{
					size_t ext_end = start + ext.size();
					if (ext_end == ctx.request_path.size() || ctx.request_path[ext_end - 1] == '/' || ctx.request_path[ext_end] == '/')
					{
						if (ctx.matched_extension == NO_EXT)
						{
							ctx.matched_extension = extensionFromString(ext);
							ctx.ext_str = ext;
							if (!location->getCGIExec().empty())
								ctx.cgi_exec = location->getCGIExec();
							//else if (idem pour cgi_on)
						}
						if (!ctx.matched_location)
							ctx.matched_location = location;
					}
				}
			}
			else if (ctx.request_path.find(route_path, 0) == 0)
			{
				size_t route_len = route_path.size();

				// Test if it's a real match, not just a partial prefix
				if (ctx.request_path.size() == route_len || ctx.request_path[route_len - 1] == '/' || ctx.request_path[route_len] == '/')
				{
					//Keep the longest match
					if (route_len > longest_match)
					{
						longest_match = route_path.size();
						ctx.matched_location = location;
					}
				}
			}
		}
		if (!ctx.matched_location)
			ctx.matched_location = &(server->getLocationAt("MAIN"));
	}

	bool	resolve(PathContext& ctx, Response& resp)
	{
		std::cout << "[resolvePath] Request path: " << ctx.request_path << std::endl;
		
		if (detectCGI(ctx))
		{
			if (!ctx.matched_location->getAlias().empty())
			{
				std::string	location_path	= ctx.matched_location->getName();
				std::string	remaining_path	= ctx.request_path.substr(location_path.size());
				ctx.resolved_path = ctx.matched_location->getAlias() + remaining_path;
			}
			else
			{
				if (!ctx.matched_location->getRoot().empty())	
				ctx.root = ctx.matched_location->getRoot();
				ctx.resolved_path = ctx.root + ctx.script_name;
			}
			if (!ctx.cgi_exec.empty())
			{
				std::string root = ctx.matched_location->getRoot();
				ctx.cgi_exec = (!root.empty() && root[root.size() -1] != '/')
				? root + '/' + ctx.cgi_exec
				: root + ctx.cgi_exec;
			}
			ctx.is_cgi= true;

			if (!normalizePath(ctx, resp))
				return false;
		}
		else
		{
			if (!normalizePath(ctx, resp))
				return false;
			if (handleConfigRedirect(ctx, resp))
				return false;
			if (!ctx.matched_location->getAlias().empty())
			{
				std::string	location_path	= ctx.matched_location->getName();
				std::string	remaining_path	= ctx.request_path.substr(location_path.size());
				ctx.resolved_path = ctx.matched_location->getAlias() + remaining_path;
			}
			else 
			{
				if (!ctx.matched_location->getRoot().empty())
					ctx.root = ctx.matched_location->getRoot();
				ctx.resolved_path = ctx.root + ctx.request_path;
			}
		}
	return validate(ctx, resp);
	}

	bool	validate(PathContext& ctx, Response& resp)
	{
		struct stat statBuf;

		if (ctx.matched_extension == UNKNOWN_EXT)
			return true;

		if (stat(ctx.resolved_path.c_str(), &statBuf) != 0)
		{
			if (errno == ENOENT)
				resp.setStatusCode(NOT_FOUND);
			else if (errno == EACCES)
				resp.setStatusCode(FORBIDDEN);
			else
				resp.setStatusCode(INTERNAL_SERVER_ERROR);
			return false;
		}

		if (ctx.is_cgi && !fileSystem::isExecutable(ctx.resolved_path))
		{	
			perror("[ERROR] Script is not executable");
			resp.setStatusCode(FORBIDDEN);
			return false;
		}

		ctx.is_directory = S_ISDIR(statBuf.st_mode);

		if (ctx.is_directory && hasTrailingSlash(ctx, resp))
			return false;

		return true;
	}

	bool	decodePath(const std::string& encoded, std::string& decoded)
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

	bool	normalizePath(PathContext& ctx, Response& resp)
	{
		std::string cage_root = ctx.matched_location->getName() == "MAIN" || ctx.matched_extension != NO_EXT 
			? "" : ctx.matched_location->getName();

		std::string	decoded_path;

		if (!decodePath(ctx.request_path, decoded_path))
		{
			std::cerr << "[ERROR] Path traversal attempt" << std::endl;
			resp.setStatusCode(FORBIDDEN);
			return false;
		}

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
					resp.setStatusCode(FORBIDDEN);
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

		ctx.request_path = temp_path;
		return true;
	}

	bool	detectCGI(PathContext& ctx)
	{
		if (!ctx.matched_location->getCGI() && ctx.cgi_exec.empty())
			return false;

		if (ctx.matched_extension == NO_EXT)
			return false;

		std::string ext_str = (ctx.matched_extension == UNKNOWN_EXT) 
			? ctx.ext_str : extensionToString(ctx.matched_extension);

		size_t start = ctx.request_path.find(ext_str);
		if (start == std::string::npos)
			return false;
		size_t ext_end = start + ext_str.size();

		ctx.script_name = (ctx.matched_extension == UNKNOWN_EXT) 
			? ctx.cgi_exec : ctx.request_path.substr(0, ext_end);

		ctx.path_info = "";

		if (ctx.matched_extension == UNKNOWN_EXT && !ctx.cgi_exec.empty())
			ctx.path_info = ctx.request_path.substr(0, ext_end);
		else
		{
			if (ext_end < ctx.request_path.size() && ctx.request_path[ext_end] == '/')
			{
				ctx.path_info = ctx.request_path.substr(ext_end);
				ctx.request_path = ctx.request_path.substr(0, ext_end);
			}
		}

		return true;
	}

	bool	handleConfigRedirect(PathContext& ctx, Response& resp)
	{
		std::string redirect_uri = ctx.matched_location->getRedirect();
		int			redirect_code = ctx.matched_location->getRedirectCode();

		if (redirect_uri.empty() || !redirect_code)
			return false;

		resp.setHeader("Location", redirect_uri);
		resp.setStatusCode(redirect_code);

		return true;
	}

	bool	hasTrailingSlash(PathContext& ctx, Response& resp)
	{
		if (ctx.request_path[ctx.request_path.size() - 1] == '/')
			return false;

		std::string redirect_uri = ctx.request_path + "/";
		if (!ctx.query.empty())
			redirect_uri += '?' + ctx.query;

		resp.setHeader("Location", redirect_uri);
		resp.setStatusCode(MOVED_PERMANENTLY);

		return true;
	}
	
	bool	hasRedirect(const Response& resp)
	{
		int status = resp.getStatusCode();

		return status == MOVED_PERMANENTLY
			|| status == FOUND
			|| status == SEE_OTHER
			|| status == TEMPORARY_REDIRECT;
	}
}

/* METHOD */

namespace method
{
	bool	dispatch(PathContext& ctx, Request& req, Response& resp)
	{
	/* 		if (resp.isCGI)
			return (true); */

		switch(req.getMethod())
		{
			case GET:
				return processGet(ctx, resp);
			case HEAD:
				return processHead(ctx, resp);
			case POST:
				return processPost(ctx, req, resp);
			case DELETE:
				return processDelete(ctx, resp);
			default: 
				resp.setStatusCode(METHOD_NOT_ALLOWED); 
				return false;
		}
		return (true);
	}

	bool	isAllowed(const PathContext& ctx, Request& req, Response& resp)
	{
		std::set<std::string> allowed = ctx.matched_location->getAllowedMethods();

		if (allowed.empty())
			return true;

		std::string method_str = methodToString(req.getMethod());

		if (allowed.find(method_str) == allowed.end())
		{
			req.setStatusCode(METHOD_NOT_ALLOWED);
			resp.setHeader("Allow", resp::buildAllowHeader(allowed));
			return false;
		}
		return true;
	}

	bool	processGet(PathContext& ctx, Response& resp)
	{
		if (ctx.is_directory)
		{
			if (!resolveIndex(ctx))
			{
				if (hasAutoIndex(ctx))
				{
					generateAutoIndex(ctx, resp);
						return true;
				}
				resp.setStatusCode(NOT_FOUND);
				return false;
			}
		}

		if (!resp::loadBody(resp, ctx.resolved_path))
			return false;
		
		if (resp.getMethod() == NOT_SET)
			resp.setMethod(GET);

		return true;	
	}

	bool	processHead(PathContext& ctx, Response& resp)
	{
		resp.setMethod(HEAD);
		processGet(ctx, resp);
		return (false);
	}

	// ???????
	bool	processPost(PathContext& ctx, const Request& req, Response& resp)
	{
		if (!upload::hasContentTypeHeader(req)) {
			resp.setStatusCode(BAD_REQUEST);	return (false);
		}

		if (!upload::isMultiformData(req)) {
			resp.setStatusCode(BAD_REQUEST);	return (false);
		}

		std::string boundary = upload::extractBoundary(req);
		if (boundary.empty()) {
			resp.setStatusCode(BAD_REQUEST);	return (false);
		}

		std::string filename = upload::extractFilename(req, boundary);
		if (filename.empty()) {
			resp.setStatusCode(BAD_REQUEST);	return (false);
		}

		filename = upload::verifyFile(ctx, filename);
		if (filename.empty()) {
			resp.setStatusCode(FORBIDDEN);		return (false);
		}

		if (!upload::saveDataToFile(req, filename)) {
			resp.setStatusCode(INTERNAL_SERVER_ERROR);	return (false);
		};

		resp.setStatusCode(CREATED);
		resp.setHeader("Content-Length", "0");
		resp.setHttpVersion(req.getHttpVersion());
		return (true);
	}

	bool	processDelete(const PathContext& ctx, Response& resp)
	{
		if (ctx.is_directory) {
			resp.setStatusCode(FORBIDDEN); return (false);
		}

		if (!fileSystem::isFile(ctx.resolved_path)) {
			resp.setStatusCode(NOT_FOUND); return (false);
		}

		std::string dirname	= fileSystem::getDirname(ctx.resolved_path);
		if (!fileSystem::isWritable(dirname) || !fileSystem::isExecutable(dirname)) {
			resp.setStatusCode(FORBIDDEN); return (false);
		}

		if (unlink(ctx.resolved_path.c_str()) != 0) {
			resp.setStatusCode(FORBIDDEN); return (false);
		}

		resp.setStatusCode(NO_CONTENT);
		return (true);
	}

	/* INDEX/DIRECTORY HANDLING */

	bool	resolveIndex(PathContext& ctx)
	{
		const std::vector<std::string> indexes = ctx.matched_location->getIndexes();

		std::string	dir_path = ctx.resolved_path;
		if (!dir_path.empty() && dir_path[dir_path.size() -1] != '/')
			dir_path += '/';

		for (size_t i = 0; i < indexes.size(); i++)
		{
			std::string test_path = dir_path + indexes[i];
			if (access(test_path.c_str(), R_OK) == 0)
			{
				ctx.resolved_path = test_path;
				ctx.is_directory = false;
				return true;
			}
		}
		return false;
	}

	bool	hasAutoIndex(const PathContext& ctx)
	{
		return ctx.matched_location->getAutoIndex();
	}

	void	generateAutoIndex(const PathContext& ctx, Response& resp)
	{
		std::string	html = ::generateAutoIndex(ctx.resolved_path);

		resp.setBody(html);
		resp.setHeader("Content-Type", "text/html");
	}
}
