#ifndef PATHCONTEXT_HPP
# define PATHCONTEXT_HPP

#include <string>
#include "HTTPenum.hpp"

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
	std::string				ext_str;
	bool					is_directory;
	bool					is_cgi;

	/* CGI ENV */
	std::string				script_name;
	std::string				path_info;
	std::string				query;

	// move to .cpp?
	PathContext()
	: matched_location(NULL)
	, matched_extension(NO_EXT)
	, is_directory(false)
	, is_cgi(false)
	{}
};

#endif
