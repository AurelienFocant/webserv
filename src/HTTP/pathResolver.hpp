#ifndef PATHRESOLVER_HPP
#define PATHRESOLVER_HPP

#include <string>
#include "HTTPenum.hpp"

class Response;
class Request;
class VirtualServer;
class Location;

struct PathContext
{
	/* Path Resolution */
	std::string				root;
	std::string				request_path;
	std::string				resolved_path;
	std::string				cgi_exec;
	const Location*			matched_location;
	t_extension				matched_extension;
	std::string				location_name;
	std::string				ext_str;
	bool					is_directory;
	bool					is_cgi;

	/* CGI ENV */
	std::string				script_name;
	std::string				path_info;
	std::string				query;

	/* Constructor */
	PathContext();

};

namespace path
{
	bool		extract(PathContext& ctx, const Request& req);
	void		matchLocation(PathContext& ctx, const VirtualServer* server);
	bool		resolve(PathContext& ctx, Response& resp);
	bool		validate(PathContext& ctx, Response& resp);

	bool		decodeUri(std::string& request_uri);
	bool		normalizePath(PathContext& ctx, Response& resp);
	bool		detectCGI(PathContext& ctx);
	bool		handleConfigRedirect(PathContext& ctx, Response& resp);
	bool		hasTrailingSlash(PathContext& ctx, Response& resp);
	bool 		isValidUriChar(char c);
	bool		hasRedirect(const Response& resp);
}

#endif