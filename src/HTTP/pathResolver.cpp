
#include "pathResolver.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "VirtualServer.hpp"
#include "../Utils/fileSystem.hpp"
#include "../Utils/httpUtils.hpp"

#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <cerrno>

struct PathContext;

PathContext::PathContext()
	: matched_location(NULL)
	, matched_extension(NO_EXT)
	, is_directory(false)
	, is_cgi(false)
{}

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

		for (std::map<std::string, Location>::const_iterator it = server->getLocations().begin(); it != server->getLocations().end(); it++)
		{
			std::string			route_path = it->first;
			const Location*		location = &(it->second);

			if (route_path.size() > 1 && route_path[route_path.size() -1] == '/')
				route_path = route_path.substr(0,route_path.size() -1);

			if (route_path[route_path.size() -1] == '$')
			{
				ext = route_path.substr(0, route_path.size() -1);
				size_t start = ctx.request_path.find(ext);
				if (start != std::string::npos)
				{
					size_t ext_end = start + ext.size();
					if (ext_end == ctx.request_path.size() || ctx.request_path[ext_end] == '/')
					{
						if (ctx.matched_extension == NO_EXT)
						{
							ctx.matched_extension = extensionFromString(ext);
							ctx.ext_str = ext;
							ctx.cgi_exec = location->getCGIExec();
							ctx.is_cgi = location->getCGI();
						}
						if (!ctx.matched_location)
							ctx.matched_location = location;
					}
				}
			}
			else if (ctx.request_path.find(route_path, 0) == 0)
			{
				size_t route_len = route_path.size();

				if (ctx.request_path.size() == route_len || ctx.request_path[route_len] == '/')
				{
					if (route_len > longest_match)
					{
						longest_match = route_path.size();
						ctx.matched_location = location;
						ctx.is_cgi = location->getCGI();
						ctx.location_name = route_path;;
						
					}
				}
			}
		}
		if (!ctx.matched_location)
			ctx.matched_location = &(server->getLocationAt("MAIN"));
	}

	bool	resolve(PathContext& ctx, Response& resp)
	{
		if (detectCGI(ctx))
		{
			if (!ctx.matched_location->getAlias().empty() && ctx.matched_location)
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
				std::string	remaining_path	= ctx.request_path.substr(ctx.location_name.size());
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

		if (ctx.is_directory && !hasTrailingSlash(ctx, resp))
			return false;

		if (!ctx.is_directory && !ctx.is_cgi)
		{
			char buf[64];
			struct tm *tm_info = gmtime(&statBuf.st_mtime);
			strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
			resp.setHeader("Last-Modified", buf); 
		}

		return true;
	}

	bool	decodePath(const std::string& encoded, std::string& decoded)
	{
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
			? "" : ctx.location_name;

		std::string	decoded_path;

		if (!decodePath(ctx.request_path, decoded_path))
		{
			std::cerr << "[ERROR] Path traversal attempt" << std::endl;
			resp.setStatusCode(BAD_REQUEST);
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
					resp.setStatusCode(BAD_REQUEST);
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
		if (ctx.request_path[ctx.request_path.size() - 1] == '/' || resp.getMethod() == POST)
			return true;

		std::string redirect_uri = ctx.request_path + "/";
		if (!ctx.query.empty())
			redirect_uri += '?' + ctx.query;

		resp.setHeader("Location", redirect_uri);
		
		resp.setStatusCode(MOVED_PERMANENTLY);

		return false;
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

